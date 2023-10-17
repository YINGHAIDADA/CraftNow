#pragma once

#include "CraftNow/Core.h"
#include "CraftNow/Events/Event.h"
#include "CraftNow/Events/ApplicationEvent.h"

#include "Window.h"

namespace CraftNow {

	class CRAFTNOW_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in Client
	Application* CreateApplication();
}


