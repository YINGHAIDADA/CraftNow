#pragma once

#include "CraftNow/Core/Core.h"

#include "Window.h"
#include "CraftNow/Core/LayerStack.h"
#include "CraftNow/Events/Event.h"
#include "CraftNow/Events/ApplicationEvent.h"

#include "CraftNow/Core/Timestep.h"

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
		void Draw();
		bool OnWindowClose(WindowCloseEvent &e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		//--------暂时---------
		/*Ref<Shader> m_Shader;
		Ref<VertexArray> m_VertexArray;

		Ref<Shader> m_BlueShader;
		Ref<VertexArray> m_SquareVA;

		OrthographicCamera m_Camera;*/
		//--------暂时---------
	private:
		static Application *s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in Client
	Application *CreateApplication();
}
