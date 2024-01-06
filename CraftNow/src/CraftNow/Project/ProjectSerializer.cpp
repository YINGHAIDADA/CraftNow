#include "cnpch.h"
#include "CraftNow/Project/ProjectSerializer.h"

#include "CraftNow/Utils/PlatformUtils.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace CraftNow {
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << (uint64_t)config.StartScene;
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.AssetRegistryPath.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		FileDialogs::createDirectory(filepath.string());
		std::ofstream fout(filepath);
		if (fout.is_open())
		{
			fout << out.c_str();
			fout.close();
			CN_CORE_TRACE("Project序列化写入“{0}”成功", filepath);
		}
		else
		{
			CN_CORE_TRACE("Project序列化写入失败");
			return false;
		}
		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			CN_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		CN_CORE_TRACE("载入工程: '{0}'", config.Name);
		config.StartScene = projectNode["StartScene"].as<uint64_t>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		if (projectNode["AssetRegistryPath"])
			config.AssetRegistryPath = projectNode["AssetRegistryPath"].as<std::string>();
		config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();
		return true;
	}
}
