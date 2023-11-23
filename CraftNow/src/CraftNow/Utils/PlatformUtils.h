#pragma once

#include <string>

namespace CraftNow {

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
		static bool createDirectory(const std::string& path);
	};

	class Time
	{
	public:
		static double GetTime();
	};
}
