#include "cnpch.h"
#include "CraftNow/Renderer/Framebuffer.h"

#include "CraftNow/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"


namespace CraftNow {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    CN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
		}

		CN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}


