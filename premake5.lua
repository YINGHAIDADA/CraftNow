workspace "CraftNow"
	architecture "x64"
	startproject "CraftNowEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
		
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/CraftNow/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/CraftNow/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/CraftNow/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/CraftNow/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/CraftNow/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/CraftNow/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/CraftNow/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/CraftNow/vendor/ImGuizmo"

group "Dependencies"
	include "Vendor/premake"
	include "CraftNow/vendor/GLFW"
	include "CraftNow/vendor/Glad"
	include "CraftNow/vendor/imgui"
	include "CraftNow/vendor/yaml-cpp"
	
group ""

include "CraftNow"
include "Sandbox"
include "CraftNowEditor"
