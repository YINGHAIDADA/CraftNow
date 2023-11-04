#include "CraftNow.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public CraftNow::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_CameraController(1600.0f / 900.0f, false)
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
		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
		};
		m_TextureVA = CraftNow::VertexArray::Create();
		float textureVertices[5 * 4] = {
			-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		};

		CraftNow::Ref<CraftNow::VertexBuffer> squareVB = CraftNow::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		CraftNow::Ref<CraftNow::VertexBuffer> TextureVB = CraftNow::VertexBuffer::Create(textureVertices, sizeof(textureVertices));

		squareVB->SetLayout({
			{ CraftNow::ShaderDataType::Float3, "a_Position" }
			});
		TextureVB->SetLayout({
			{ CraftNow::ShaderDataType::Float3, "a_Position" },
			{ CraftNow::ShaderDataType::Float2, "a_TexCoord" }
			});


		m_SquareVA->AddVertexBuffer(squareVB);
		m_TextureVA->AddVertexBuffer(TextureVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		CraftNow::Ref<CraftNow::IndexBuffer> squareIB = CraftNow::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		CraftNow::Ref<CraftNow::IndexBuffer> textureIB = CraftNow::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		m_SquareVA->SetIndexBuffer(squareIB);
		m_TextureVA->SetIndexBuffer(textureIB);


		//--------暂时渲染三角形---------

		//--------暂时shader---------
		
		m_ShaderLibrary.Load("assets/shaders/Sanjiao.glsl");
		m_ShaderLibrary.Load("assets/shaders/FloatSquare.glsl");

		m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

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
		m_CameraController.OnUpdate(ts);
		////CN_INFO("ExampleLayer::Update");
		////CN_TRACE("间隔时间：{0}s {1}ms", ts.GetSeconds(), ts.GetMilliseconds());

		//--------------渲染-----------------
		CraftNow::RenderCommand::SetClearColor({ 0.27f, 0.447f, 0.682f, 1 });
		CraftNow::RenderCommand::Clear();

		CraftNow::Renderer::BeginScene(m_CameraController.GetCamera());

		//-----------转换矩阵---------------------------

		/*glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		auto FloatShader = m_ShaderLibrary.Get("FloatSquare");
		FloatShader->Bind();
		FloatShader->SetFloat3("u_Color", m_SQColor);
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				glm::vec3 pos(x * 0.11f - 0.5f, y * 0.11f - 0.5f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				CraftNow::Renderer::Submit(FloatShader, m_SquareVA, transform);
			}
		}*/

		//-----------转换矩阵---------------------------
		m_Texture = CraftNow::Texture2D::Create("assets/textures/yinghai_alpha.png");

		auto textureShader = m_ShaderLibrary.Get("Texture");
		textureShader->Bind();
		//TODO:对于纹理插槽后续需要完善，现在暂时设为0
		textureShader->SetInt("u_Texture", 0);


		m_Texture->Bind();
		CraftNow::Renderer::Submit(textureShader, m_TextureVA);

		//auto SanjiaoShader = m_ShaderLibrary.Get("Sanjiao");
		//CraftNow::Renderer::Submit(SanjiaoShader, m_VertexArray);

		CraftNow::Renderer::EndScene();

	}

	void OnEvent(CraftNow::Event& event) override
	{
		//CN_TRACE("{0}:{1}", m_DebugName, event);
		m_CameraController.OnEvent(event);
	}
private:
	CraftNow::ShaderLibrary m_ShaderLibrary;

	CraftNow::Ref<CraftNow::VertexArray> m_VertexArray;
	CraftNow::Ref<CraftNow::VertexArray> m_SquareVA;
	CraftNow::Ref<CraftNow::VertexArray> m_TextureVA;

	CraftNow::Ref<CraftNow::Texture2D> m_Texture;

	CraftNow::OrthographicCameraController m_CameraController;

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