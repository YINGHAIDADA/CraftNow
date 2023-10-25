#include "cnpch.h"
#include "CraftNow/Renderer/VertexArray.h"

#include "CraftNow/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace CraftNow {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    CN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}

		CN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}