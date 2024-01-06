#pragma once

#include "AssetMetadata.h"

namespace CraftNow {

	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};

}
