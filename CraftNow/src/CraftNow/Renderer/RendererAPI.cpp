#include "cnpch.h"

#include "CraftNow/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace CraftNow {

	//TODO: 决定使用什么API渲染
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    CN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		}

		CN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}