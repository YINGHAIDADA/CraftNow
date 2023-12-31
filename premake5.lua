﻿include "Dependencies.lua"

workspace "CraftNow"
	architecture "x86_64"
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

group "Dependencies"
	include "Vendor/premake"
	include "CraftNow/vendor/GLFW"
	include "CraftNow/vendor/Glad"
	include "CraftNow/vendor/imgui"
	include "CraftNow/vendor/yaml-cpp"
	include "CraftNow/vendor/Box2D"
	
group ""

group "Core"
	include "CraftNow"
	include "CraftNowEditor"
group ""

group "Test"
	include "Sandbox"
group ""
