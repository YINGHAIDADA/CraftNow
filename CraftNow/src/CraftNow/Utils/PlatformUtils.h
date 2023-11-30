#pragma once

#include <string>
#include <filesystem>

namespace CraftNow {

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
		static bool createDirectory(const std::string& path);
		static bool isFileExist(const std::filesystem::path& directory, const std::string& targetFileName);
		static void copyFile(const std::filesystem::path& source, const std::filesystem::path& destination);
	};

	class Time
	{
	public:
		static double GetTime();
	};
}
