#pragma once

#include <string>
#include <filesystem>

#include "CraftNow/Core/Core.h"

#include "CraftNow/Asset/RuntimeAssetManager.h"
#include "CraftNow/Asset/EditorAssetManager.h"

namespace CraftNow {
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		AssetHandle StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath; // Relative to AssetDirectory
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			CN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetProjectDirectory();
		}

		static std::filesystem::path GetActiveAssetDirectory()
		{
			CN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			CN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetRegistryPath();
		}

		// TODO: move to asset manager when we have one
		static std::filesystem::path GetActiveAssetFileSystemPath(const std::filesystem::path& path)
		{
			CN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetFileSystemPath(path);
		}

		const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }
		std::filesystem::path GetAssetDirectory() { return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory; }
		std::filesystem::path GetAssetRegistryPath() { return GetAssetDirectory() / s_ActiveProject->m_Config.AssetRegistryPath; }
		// TODO: move to asset manager when we have one
		std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }

		//TODO: 改为返回绝对地址
		std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }
		Ref<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		Ref<RuntimeAssetManager> GetRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManager); }
		Ref<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		Ref<AssetManagerBase> m_AssetManager;

		inline static Ref<Project> s_ActiveProject;
	};
}
