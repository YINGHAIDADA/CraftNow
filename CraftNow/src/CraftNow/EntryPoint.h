#pragma once

#ifdef CN_PLATFORM_WINDOWS

extern CraftNow::Application* CraftNow::CreateApplication();

int main(int argc, char** argv)
{
	CraftNow::Log::Init();
	CN_CORE_WARN("Initialized Log!");
	CN_INFO("Hello!");

	auto app = CraftNow::CreateApplication();
	app->Run();
	delete app;
}

#endif