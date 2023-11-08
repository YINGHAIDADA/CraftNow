#pragma once
#include "CraftNow/Core/Core.h"
#include "CraftNow/Core/Application.h"

#ifdef CN_PLATFORM_WINDOWS

extern CraftNow::Application* CraftNow::CreateApplication();

int main(int argc, char** argv)
{
	CraftNow::Log::Init();
	CN_CORE_WARN("Initialized Log!");
	CN_INFO("Hello!");

	CN_PROFILE_BEGIN_SESSION("Startup", "CraftNowProfile-Startup.json");
	auto app = CraftNow::CreateApplication();
	CN_PROFILE_END_SESSION();

	CN_PROFILE_BEGIN_SESSION("Runtime", "CraftNowProfile-Runtime.json");
	app->Run();
	CN_PROFILE_END_SESSION();

	CN_PROFILE_BEGIN_SESSION("Shutdown", "CraftNowProfile-Shutdown.json");
	delete app;
	CN_PROFILE_END_SESSION();
}

#endif