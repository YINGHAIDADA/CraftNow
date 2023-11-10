project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/CraftNow/vendor/spdlog/include",
		"%{wks.location}/CraftNow/src",
		"%{wks.location}/CraftNow/vendor",
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
