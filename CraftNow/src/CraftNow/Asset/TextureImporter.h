#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "CraftNow/Renderer/Texture.h"

namespace CraftNow {

	class TextureImporter
	{
	public:
		// AssetMetadata filepath is relative to project asset directory
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);

		// Reads file directly from filesystem
		// (i.e. path has to be relative / absolute to working directory)
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};



}
