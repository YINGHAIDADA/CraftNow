#pragma once

#include "CraftNow/Core/Buffer.h"

namespace CraftNow {

	class FileSystem
	{
	public:
		// TODO: move to FileSystem class
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};
}
