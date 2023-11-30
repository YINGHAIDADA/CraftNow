#include "cnpch.h"
#include "CraftNow/Utils/PlatformUtils.h"
#include "CraftNow/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace CraftNow {

	double Time::GetTime()
	{
		return glfwGetTime();
	}


	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()));
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();

	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()));
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	bool FileDialogs::createDirectory(const std::string& path)
	{
		namespace fs = std::filesystem;

		fs::path directoryPath = fs::path(path).parent_path();

		try {
			if (!fs::exists(directoryPath)) {
				// 递归创建目录
				fs::create_directories(directoryPath);
				CN_CORE_TRACE("创建文件夹：{0}", directoryPath.string());
			}
			else
			{
				CN_CORE_TRACE("路径：{0}, 已存在", directoryPath.string());
			}
			return true;
		}
		catch (const std::exception& e) {
			CN_CORE_ERROR("创建文件夹失败: {0}", e.what());
			return false;
		}
	}

	bool FileDialogs::isFileExist(const std::filesystem::path& directory, const std::string& targetFileName)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
			if (entry.is_regular_file() && entry.path().filename().string() == targetFileName) {
				// 找到目标文件
				return true;
			}
		}
		// 没有找到目标文件
		return false;
	}

	void FileDialogs::copyFile(const std::filesystem::path& source, const std::filesystem::path& destination)
	{
		try {
			std::filesystem::copy(source, destination / source.filename(), std::filesystem::copy_options::overwrite_existing);
			CN_CORE_TRACE("文件复制成功！");
		}
		catch (const std::filesystem::filesystem_error& e) {
			CN_CORE_ERROR("文件复制失败！..");
		}
	}
}
