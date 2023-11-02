#include "CraftNow.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public CraftNow::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		////--------暂时渲染三角形---------
		m_VertexArray = CraftNow::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 1.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 1.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 1.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		CraftNow::Ref<CraftNow::VertexBuffer> vertexBuffer = CraftNow::VertexBuffer::Create(vertices, sizeof(vertices));

		vertexBuffer->SetLayout({
			{CraftNow::ShaderDataType::Float3, "a_Position"},
			{CraftNow::ShaderDataType::Float4, "a_Color"}
			});
		m_VertexArray->AddVertexBuffer(vertexBuffer);


		uint32_t indices[3] = { 0, 1, 2 };
		CraftNow::Ref<CraftNow::IndexBuffer> indexBuffer = CraftNow::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		m_SquareVA = CraftNow::VertexArray::Create();
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};

		CraftNow::Ref<CraftNow::VertexBuffer> squareVB = CraftNow::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

		squareVB->SetLayout({
			{ CraftNow::ShaderDataType::Float3, "a_Position" },
			{ CraftNow::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		CraftNow::Ref<CraftNow::IndexBuffer> squareIB = CraftNow::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_SquareVA->SetIndexBuffer(squareIB);


		//--------暂时渲染三角形---------

		//--------暂时shader---------
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";

		m_Shader = CraftNow::Shader::Create(vertexSrc, fragmentSrc);


		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FloatShader = CraftNow::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		std::string textureShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string textureShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;
			
			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";

		m_TextureShader = CraftNow::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc);

		//--------暂时shader---------
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("ColorSetting");
		ImGui::ColorEdit3("Color", glm::value_ptr(m_SQColor));
		ImGui::End();
	}

	void OnUpdate(CraftNow::Timestep ts) override
	{
		//CN_INFO("ExampleLayer::Update");
		//CN_TRACE("间隔时间：{0}s {1}ms", ts.GetSeconds(), ts.GetMilliseconds());

		//----------鼠标拖动画布事件---------------------
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse)
		{
			if (CraftNow::Input::IsMouseButtonPressed(CraftNow::Mouse::ButtonLeft))
			{
				auto now_Position = CraftNow::Input::GetMousePosition();
				m_CameraPosition.x -= (now_Position.x - m_mousePositon.x) * 0.12f * ts;
				m_CameraPosition.y += (now_Position.y - m_mousePositon.y) * 0.12f * ts;
			}
		}
		//----------鼠标拖动画布事件---------------------

		//----------键盘相机移动事件---------------------
		if (!io.WantCaptureKeyboard)
		{
			if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Left) || CraftNow::Input::IsKeyPressed(CraftNow::Key::A))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
					m_CameraPosition.x -= m_CameraMoveSpeed * 0.1f * ts;
				else
					m_CameraPosition.x -= m_CameraMoveSpeed * ts;
			else if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Right) || CraftNow::Input::IsKeyPressed(CraftNow::Key::D))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
					m_CameraPosition.x += m_CameraMoveSpeed * 0.1f * ts;
				else
					m_CameraPosition.x += m_CameraMoveSpeed * ts;

			if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Up) || CraftNow::Input::IsKeyPressed(CraftNow::Key::W))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
					m_CameraPosition.y += m_CameraMoveSpeed * 0.1f * ts;
				else
					m_CameraPosition.y += m_CameraMoveSpeed * ts;
			else if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Down) || CraftNow::Input::IsKeyPressed(CraftNow::Key::S))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
					m_CameraPosition.y -= m_CameraMoveSpeed * 0.1f * ts;
				else
					m_CameraPosition.y -= m_CameraMoveSpeed * ts;

			if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			else if (CraftNow::Input::IsKeyPressed(CraftNow::Key::E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Space) || CraftNow::Input::IsMouseButtonPressed(CraftNow::Mouse::ButtonRight))
			{
				m_CameraPosition.x = 0.0f; m_CameraPosition.y = 0.0f;
				m_CameraRotation = 0.0f;
			}
		}
		
		//----------键盘相机移动事件---------------------

		CraftNow::RenderCommand::SetClearColor({ 0.27f, 0.447f, 0.682f, 1 });
		CraftNow::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		CraftNow::Renderer::BeginScene(m_Camera);

		//-----------转换矩阵---------------------------
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_FloatShader->Bind();
		m_FloatShader->SetFloat3("u_Color", m_SQColor);


		/*for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				glm::vec3 pos(x * 0.11f - 0.5f, y * 0.11f - 0.5f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				CraftNow::Renderer::Submit(m_FloatShader, m_SquareVA, transform);
			}
		}*/
		//-----------转换矩阵---------------------------
		m_Texture = CraftNow::Texture2D::Create("assets/textures/yinghai_alpha.png");

		m_TextureShader->Bind();
		//TODO:对于纹理插槽后续需要完善，现在暂时设为0
		m_TextureShader->SetInt("u_Texture", 0);
		m_Texture->Bind();
		CraftNow::Renderer::Submit(m_TextureShader, m_SquareVA);


		//CraftNow::Renderer::Submit(m_Shader, m_VertexArray);

		CraftNow::Renderer::EndScene();

		m_mousePositon = CraftNow::Input::GetMousePosition();
	}

	void OnEvent(CraftNow::Event& event) override
	{
		//CN_TRACE("{0}:{1}", m_DebugName, event);
	}
private:
	CraftNow::Ref<CraftNow::Shader> m_Shader;
	CraftNow::Ref<CraftNow::VertexArray> m_VertexArray;

	CraftNow::Ref<CraftNow::Shader> m_FloatShader, m_TextureShader;
	CraftNow::Ref<CraftNow::VertexArray> m_SquareVA;

	CraftNow::Ref<CraftNow::Texture2D> m_Texture;

	CraftNow::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
	float m_CameraMoveSpeed = 4.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec2 m_mousePositon;

	glm::vec3 m_SQColor = { 0.8f, 0.8f, 0.8f };

};

class Sandbox : public CraftNow::Application
{
public:
	Sandbox() 
	{
		//TODO:不添加任何层，imgui会出现渲染残影错误
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}

private:

};


CraftNow::Application* CraftNow::CreateApplication() 
{
	return new Sandbox();
}