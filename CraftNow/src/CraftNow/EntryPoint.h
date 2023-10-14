#pragma once

#ifdef CN_PLATFORM_WINDOWS

extern CraftNow::Application* CraftNow::CreateApplication();

int main(int argc, char** argv)
{
	auto app = CraftNow::CreateApplication();
	app->Run();
	delete app;
}

#endif