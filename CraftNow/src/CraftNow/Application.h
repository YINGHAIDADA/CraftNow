#pragma once

#include "Core.h"

namespace CraftNow {

	class CRAFTNOW_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in Client
	Application* CreateApplication();
}


