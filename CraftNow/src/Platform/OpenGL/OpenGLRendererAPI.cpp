#include "cnpch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace CraftNow {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         CN_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       CN_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          CN_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: CN_CORE_TRACE(message); return;
		}

		CN_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		CN_PROFILE_FUNCTION();

		//#ifdef CN_DEBUG
		//glEnable(GL_DEBUG_OUTPUT);
		//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		//glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		//#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//用来开启更新深度缓冲区的功能, 根据Z轴判断哪些渲染在前
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		//TODO: 如果为0，则使用 IndexBuffer ()-> getCount ()。也就是说，如果你没有在一个帧中绘制任何四边形(indexcount = = 0) ，Indexed 将获取存储的索引缓冲区的值，并将索引计数(IndexBuffer ()-> getCount ())(这在内存中仍然有效)以及先前存储的顶点数据(因为它已经被缓冲到 gpu，导致最后存储的“帧”被重新渲染。
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

}

