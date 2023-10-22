#include "cnpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace CraftNow {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		CN_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CN_CORE_ASSERT(status, "初始化Glad失败!");

		CN_CORE_INFO("OpenGL Info:");
		CN_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		CN_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		CN_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		CN_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "CraftNow requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
