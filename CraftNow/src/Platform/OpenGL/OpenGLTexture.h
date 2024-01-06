#pragma once

#include "CraftNow/Renderer/Texture.h"

#include <glad/glad.h>

namespace CraftNow {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification, Buffer data = Buffer());
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual const std::string& GetPath() const override { return m_Path; }

		virtual void SetData(Buffer data) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}
	private:
		TextureSpecification m_Specification;

		//TODO:未来可能对热重载功能，重写这部分
		std::string m_Path;
		bool m_IsLoaded = false;

		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

}
