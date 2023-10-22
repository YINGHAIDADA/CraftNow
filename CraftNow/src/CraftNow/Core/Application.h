#pragma once

#include "CraftNow/Core/Core.h"

#include "Window.h"
#include "CraftNow/Core/LayerStack.h"
#include "CraftNow/Events/Event.h"
#include "CraftNow/Events/ApplicationEvent.h"

#include "CraftNow/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace CraftNow {

	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void OnEvent(Event &e);

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		inline static Application &Get() { return *s_Instance; }
		inline Window &GetWindow() { return *m_Window; }

	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent &e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		//--------暂时渲染三角形---------
		unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
		//--------暂时渲染三角形---------
	private:
		static Application *s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in Client
	Application *CreateApplication();
}
