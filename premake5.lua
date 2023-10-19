workspace "CraftNow"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "CraftNow/vendor/GLFW/include"

include "CraftNow/vendor/GLFW"

project "CraftNow"
	location "CraftNow"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "cnpch.h"
	pchsource "CraftNow/src/cnpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CN_PLATFORM_WINDOWS",
			"CN_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines
		{
			"CN_DEBUG",
			"CN_ENABLE_ASSERTS"
		}
		staticruntime "off"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "CN_RELEASE"
		staticruntime "off"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "CN_DIST"
		staticruntime "off"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

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
		"CraftNow/src"
	}

	links
	{
		"CraftNow"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CN_DEBUG"
		staticruntime "off"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "CN_RELEASE"
		staticruntime "off"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "CN_DIST"
		staticruntime "off"
		runtime "Release"
		optimize "On"