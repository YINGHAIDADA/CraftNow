#include "cnpch.h"
#include "CraftNow/Core/Application.h"

#include "CraftNow/Renderer/Renderer.h"

#include "CraftNow/Core/Input.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

#include <GLFW/glfw3.h>

namespace CraftNow
{

	Application *Application::s_Instance = nullptr;

	Application::Application()
		//: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		CN_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}

	void Application::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		//dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		// 从最上层开始迭代检查事件触发
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			//TODO:最顶层收不到事件
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
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

	void Application::Draw()
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

			// 从最底开始层更新每一层
			for (Layer *layer : m_LayerStack)
				layer->OnUpdate(timestep);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{	
		return false;
	}
}