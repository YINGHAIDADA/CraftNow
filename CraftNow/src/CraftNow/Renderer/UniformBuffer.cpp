#include "cnpch.h"
#include "CraftNow/Renderer/UniformBuffer.h"

#include "CraftNow/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace CraftNow {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    CN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		CN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}



