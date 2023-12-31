﻿#include "cnpch.h"

#include "Platform/Windows/WindowsWindow.h"

#include "CraftNow/Events/ApplicationEvent.h"
#include "CraftNow/Events/MouseEvent.h"
#include "CraftNow/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include "CraftNow/Renderer/Renderer.h"

#include "CraftNow/Core/Application.h"
#include <stb_image.h>

namespace CraftNow {

	float Window::s_HighDPIScaleFactor = 1.0f;

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		CN_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		CN_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		CN_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		CN_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		CN_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount == 0)
		{
			CN_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			CN_CORE_ASSERT(success, "GLFW初始化失败!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			CN_PROFILE_SCOPE("glfwCreateWindow");

			//---------适配分辨率缩放---------------
			#if 1
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			float xscale, yscale;
			glfwGetMonitorContentScale(monitor, &xscale, &yscale);

			if (xscale > 1.0f || yscale > 1.0f)
			{
				s_HighDPIScaleFactor = xscale;
				glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
			}
			#endif
			//--------------------------------

			#if defined(CN_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif
			m_Window  = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}
		//根据平台不同抽象出渲染API
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		//------------自定义标题栏-----------

		auto& specification = Application::Get().GetSpecification();

		//会使得点击事件失效报错
		//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		if (specification.CustomTitlebar)
		{
			glfwWindowHint(GLFW_TITLEBAR, false);

			// NOTE: Undecorated windows are probably
			//       also desired, so make this an option
			// glfwWindowHint(GLFW_DECORATED, false);
		}
		//-------------------------------------------

		//---------------窗口居中显示---------------------
		if (specification.CenterWindow)
		{
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

			int monitorX, monitorY;
			glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);

			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);



			glfwSetWindowPos(m_Window,
				monitorX + (videoMode->width - specification.Width) / 2,
				monitorY + (videoMode->height - specification.Height) / 2);

			glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, specification.WindowResizeable ? GLFW_TRUE : GLFW_FALSE);

			glfwShowWindow(m_Window);
		}
		//-----------------------------------------------------------

		// 设置图标
		GLFWimage icon;
		int channels;
		if (!specification.IconPath.empty())
		{
			std::string iconPathStr = specification.IconPath.string();
			icon.pixels = stbi_load(iconPathStr.c_str(), &icon.width, &icon.height, &channels, 4);
			glfwSetWindowIcon(m_Window, 1, &icon);
			stbi_image_free(icon.pixels);
		}

		m_Data.instance = &Application::Get();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//标题栏点击检测
		if(specification.CustomTitlebar)
		{
			//glfwSetWindowUserPointer(m_Window, &(Application::Get()));
			glfwSetTitlebarHitTestCallback(m_Window, [](GLFWwindow* window, int x, int y, int* hit)
				{
					WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));
					*hit = data.instance->IsTitleBarHovered();
				});
		}


		// 设置 GLFW 回调函数
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *( (WindowData*)glfwGetWindowUserPointer(window) );
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
				
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));
			WindowCloseEvent event;

			data.EventCallback(event);
		});
		
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *((WindowData*)glfwGetWindowUserPointer(window));
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{

		glfwDestroyWindow(m_Window);
		//glfwTerminate();

	}

	void WindowsWindow::OnUpdate()
	{

		glfwPollEvents();
		m_Context->SwapBuffers();

	}

	void WindowsWindow::SetVSync(bool enabled)
	{

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}
