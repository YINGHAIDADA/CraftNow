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

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			CN_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "CraftNow Application";
		uint32_t Width = 1600;
		uint32_t Height = 900;

		std::filesystem::path IconPath;

		bool WindowResizeable = true;

		// Uses custom Walnut titlebar instead
		// of Windows default
		bool CustomTitlebar = false;

		// Window will be created in the center
		// of primary monitor
		bool CenterWindow = true;

		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		~Application();

		void OnEvent(Event &e);

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		Window &GetWindow() { return *m_Window; }
		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application &Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		void SubmitToMainThread(const std::function<void()>& function);

	private:
		void Init();
		void Run();
		bool OnWindowClose(WindowCloseEvent &e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ExecuteMainThreadQueue();

	private:
		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application *s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in Client
	Application *CreateApplication();
}
