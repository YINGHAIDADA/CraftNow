#pragma once

namespace CraftNow {

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();
		//~Shader() = default;

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_RendererID;
	};
}