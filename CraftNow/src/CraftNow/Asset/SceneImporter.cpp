#include "cnpch.h"
#include "SceneImporter.h"

#include "CraftNow/Project/Project.h"
#include "CraftNow/Scene/SceneSerializer.h"

#include <stb_image.h>

namespace CraftNow {

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		CN_PROFILE_FUNCTION();

		return LoadScene(Project::GetActiveAssetDirectory() / metadata.FilePath);
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		CN_PROFILE_FUNCTION();

		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);
		return scene;
	}

	void SceneImporter::SaveScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

}
