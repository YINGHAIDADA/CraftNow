workspace "CraftNow"
	architecture "x64"
	startproject "CraftNowEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "CraftNow/vendor/GLFW/include"
IncludeDir["Glad"] = "CraftNow/vendor/Glad/include"
IncludeDir["ImGui"] = "CraftNow/vendor/imgui"
IncludeDir["glm"] = "CraftNow/vendor/glm"
IncludeDir["stb_image"] = "CraftNow/vendor/stb_image"
IncludeDir["entt"] = "CraftNow/vendor/entt/include"

group "Dependencies"
	include "CraftNow/vendor/GLFW"
	include "CraftNow/vendor/Glad"
	include "CraftNow/vendor/imgui"
	
group ""

project "CraftNow"
	location "CraftNow"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "cnpch.h"
	pchsource "CraftNow/src/cnpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"CN_PLATFORM_WINDOWS",
			"CN_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "CN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CN_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "CN_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"CraftNow/vendor/spdlog/include",
		"CraftNow/src",
		"CraftNow/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links
	{
		"CraftNow"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"CN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CN_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "CN_DIST"
		runtime "Release"
		optimize "on"


project "CraftNowEditor"
	location "CraftNowEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"CraftNow/vendor/spdlog/include",
		"CraftNow/src",
		"CraftNow/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links
	{
		"CraftNow"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"CN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CN_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "CN_DIST"
		runtime "Release"
		optimize "on"