#include "cnpch.h"
#include "CraftNow/Core/Application.h"

#include "CraftNow/Renderer/Renderer.h"

#include "CraftNow/Core/Input.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

#include <GLFW/glfw3.h>
#include <stb_image.h>

namespace CraftNow
{
	//TODO: 可能更改静态实例化方式
	Application *Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		CN_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		Init();

	}

	Application::~Application()
	{
		s_Instance = nullptr;
	}

	void Application::Init()
	{
		m_Window = Window::Create();
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		//------------自定义标题栏-----------
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		if (m_Specification.CustomTitlebar)
		{
			glfwWindowHint(GLFW_TITLEBAR, false);

			// NOTE: Undecorated windows are probably
			//       also desired, so make this an option
			// glfwWindowHint(GLFW_DECORATED, false);
		}
		//-------------------------------------------

		//---------------窗口居中显示---------------------
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

		int monitorX, monitorY;
		glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		if (m_Specification.CenterWindow)
		{
			glfwSetWindowPos((GLFWwindow*)m_Window->GetNativeWindow(),
				monitorX + (videoMode->width - m_Specification.Width) / 2,
				monitorY + (videoMode->height - m_Specification.Height) / 2);

			glfwSetWindowAttrib((GLFWwindow*)m_Window->GetNativeWindow(), GLFW_RESIZABLE, m_Specification.WindowResizeable ? GLFW_TRUE : GLFW_FALSE);
		}

		glfwShowWindow((GLFWwindow*)m_Window->GetNativeWindow());
		//-----------------------------------------------------------

		// 设置图标
		GLFWimage icon;
		int channels;
		if (!m_Specification.IconPath.empty())
		{
			std::string iconPathStr = m_Specification.IconPath.string();
			icon.pixels = stbi_load(iconPathStr.c_str(), &icon.width, &icon.height, &channels, 4);
			glfwSetWindowIcon((GLFWwindow*)m_Window->GetNativeWindow(), 1, &icon);
			stbi_image_free(icon.pixels);
		}

		//标题栏点击检测
		/*glfwSetWindowUserPointer((GLFWwindow*)m_Window->GetNativeWindow(), this);
		glfwSetTitlebarHitTestCallback((GLFWwindow*)m_Window->GetNativeWindow(), [](GLFWwindow* window, int x, int y, int* hit)
			{
				Application* app = (Application*)glfwGetWindowUserPointer(window);
				*hit = app->IsTitleBarHovered();
			});*/

		//初始化ImGui
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		// 从最上层开始迭代检查事件触发
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer *layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer *overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{

	}

	void Application::Run()
	{

		while (m_Running)
		{
			//暂时使用GLFW的时间方法
			float time = (float)glfwGetTime();
			
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					CN_PROFILE_SCOPE("LayerStack OnUpdate");
					// 从最底开始层更新每一层
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					CN_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}


			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{	
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{

	}

}
