#include "cnpch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"

#include "CraftNow/Core/Application.h"
#include "CraftNow/Core/Timer.h"
#include "CraftNow/Core/Buffer.h"
#include "CraftNow/Core/FileSystem.h"

#include "CraftNow/Project/Project.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

#include <FileWatch.h>

namespace CraftNow {

	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Char", ScriptFieldType::Char },
		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },
		{ "System.Byte", ScriptFieldType::Byte },
		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong },

		{ "CraftNow.Vector2", ScriptFieldType::Vector2 },
		{ "CraftNow.Vector3", ScriptFieldType::Vector3 },
		{ "CraftNow.Vector4", ScriptFieldType::Vector4 },

		{ "CraftNow.Entity", ScriptFieldType::Entity },
	};

	namespace Utils {

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
		{
			ScopedBuffer fileData = FileSystem::ReadFileBinary(assemblyPath);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData.As<char>(), fileData.Size(), 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			if (loadPDB)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					ScopedBuffer pdbFileData = FileSystem::ReadFileBinary(pdbPath);
					mono_debug_open_image_from_memory(image, pdbFileData.As<const mono_byte>(), pdbFileData.Size());
					CN_CORE_INFO("Loaded PDB {}", pdbPath);
				}
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
				CN_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				CN_CORE_ERROR("Unknown type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}

	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;

		#ifdef CN_DEBUG
		bool EnableDebugging = true;
		#else
		bool EnableDebugging = false;
		#endif
		// Runtime

		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_SEData = nullptr;

	static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!s_SEData->AssemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_SEData->AssemblyReloadPending = true;

			Application::Get().SubmitToMainThread([]()
				{
					s_SEData->AppAssemblyFileWatcher.reset();
					ScriptEngine::ReloadAssembly();
				});
		}
	}

	void ScriptEngine::Init()
	{
		s_SEData = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();

		bool status = LoadAssembly("resources/Scripts/ScriptCore.dll");
		if (!status)
		{
			CN_CORE_ERROR("[ScriptEngine] 无法加载ScriptCore程序集");
			return;
		}

		auto scriptModulePath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetConfig().ScriptModulePath;
		status = LoadAppAssembly(scriptModulePath);
		if (!status)
		{
			CN_CORE_ERROR("[ScriptEngine] 无法加载 Game 程序集.");
			return;
		}

		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		// 检索并实例化类
		s_SEData->EntityClass = ScriptClass("CraftNow", "Entity", true);
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_SEData;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		if (s_SEData->EnableDebugging)
		{
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		MonoDomain* rootDomain = mono_jit_init("JITRuntime");
		CN_CORE_ASSERT(rootDomain);

		// Store the root domain pointer
		s_SEData->RootDomain = rootDomain;

		if (s_SEData->EnableDebugging)
			mono_debug_domain_create(s_SEData->RootDomain);

		mono_thread_set_main(mono_thread_current());
	}

	void ScriptEngine::ShutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_SEData->AppDomain);
		s_SEData->AppDomain = nullptr;

		mono_jit_cleanup(s_SEData->RootDomain);
		s_SEData->RootDomain = nullptr;
	}

	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			CN_CORE_TRACE("{}.{}", nameSpace, name);
		}
	}

	bool ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_SEData->AppDomain = mono_domain_create_appdomain("ScriptRuntime", nullptr);
		mono_domain_set(s_SEData->AppDomain, true);

		s_SEData->CoreAssemblyFilepath = filepath;
		s_SEData->CoreAssembly = Utils::LoadMonoAssembly(filepath, s_SEData->EnableDebugging);
		//PrintAssemblyTypes(s_SEData->CoreAssembly);
		if (s_SEData->CoreAssembly == nullptr)
			return false;

		s_SEData->CoreAssemblyImage = mono_assembly_get_image(s_SEData->CoreAssembly);
		return true;
	}

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_SEData->AppAssemblyFilepath = filepath;
		s_SEData->AppAssembly = Utils::LoadMonoAssembly(filepath, s_SEData->EnableDebugging);
		if (s_SEData->AppAssembly == nullptr)
			return false;

		s_SEData->AppAssemblyImage = mono_assembly_get_image(s_SEData->AppAssembly);

		s_SEData->AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileSystemEvent);
		s_SEData->AssemblyReloadPending = false;
		return true;
	}

	void ScriptEngine::ReloadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_SEData->AppDomain);

		LoadAssembly(s_SEData->CoreAssemblyFilepath);
		LoadAppAssembly(s_SEData->AppAssemblyFilepath);
		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		// Retrieve and instantiate class
		s_SEData->EntityClass = ScriptClass("CraftNow", "Entity", true);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_SEData->SceneContext = scene;
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_SEData->EntityClasses.find(fullClassName) != s_SEData->EntityClasses.end();
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntityClassExists(sc.ClassName))
		{
			UUID entityID = entity.GetUUID();

			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_SEData->EntityClasses[sc.ClassName], entity);
			s_SEData->EntityInstances[entityID] = instance;

			// 从编辑器复制 Fields 到 脚本运行时
			if (s_SEData->EntityScriptFields.find(entityID) != s_SEData->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_SEData->EntityScriptFields.at(entityID);
				for (const auto& [name, fieldInstance] : fieldMap)
					instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
			}

			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.GetUUID();
		if (s_SEData->EntityInstances.find(entityUUID) != s_SEData->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_SEData->EntityInstances[entityUUID];
			instance->InvokeOnUpdate((float)ts);
		}
		else
		{
			CN_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
		}
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_SEData->SceneContext;
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID)
	{
		auto it = s_SEData->EntityInstances.find(entityID);
		if (it == s_SEData->EntityInstances.end())
			return nullptr;

		return it->second;
	}


	Ref<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		if (s_SEData->EntityClasses.find(name) == s_SEData->EntityClasses.end())
			return nullptr;

		return s_SEData->EntityClasses.at(name);
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_SEData->SceneContext = nullptr;

		s_SEData->EntityInstances.clear();
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_SEData->EntityClasses;
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		CN_CORE_ASSERT(entity);

		UUID entityID = entity.GetUUID();
		return s_SEData->EntityScriptFields[entityID];
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_SEData->EntityClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_SEData->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_SEData->CoreAssemblyImage, "CraftNow", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_SEData->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_SEData->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			MonoClass* monoClass = mono_class_from_name(s_SEData->AppAssemblyImage, nameSpace, className);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity)
				continue;

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
			s_SEData->EntityClasses[fullName] = scriptClass;


			// This routine is an iterator routine for retrieving the fields in a class.
			// You must pass a gpointer that points to zero and is treated as an opaque handle
			// to iterate over all of the elements. When no more values are available, the return value is NULL.

			int fieldCount = mono_class_num_fields(monoClass);
			CN_CORE_WARN("{}.{} has {} fields:", nameSpace, className, fieldCount);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					CN_CORE_WARN("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));

					scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}

		}

		//auto& entityClasses = s_SEData->EntityClasses;

		//mono_field_get_value()

	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_SEData->CoreAssemblyImage;
	}


	MonoObject* ScriptEngine::GetManagedInstance(UUID uuid)
	{
		CN_CORE_ASSERT(s_SEData->EntityInstances.find(uuid) != s_SEData->EntityInstances.end());
		return s_SEData->EntityInstances.at(uuid)->GetManagedObject();
	}

	MonoString* ScriptEngine::CreateString(const char* string)
	{
		return mono_string_new(s_SEData->AppDomain, string);
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_SEData->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(isCore ? s_SEData->CoreAssemblyImage : s_SEData->AppAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		return mono_runtime_invoke(method, instance, params, &exception);
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_SEData->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// Call Entity constructor
		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &ts;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);
		return true;
	}

}
