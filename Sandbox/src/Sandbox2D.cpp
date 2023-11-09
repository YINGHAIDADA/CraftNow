#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	CN_PROFILE_FUNCTION();

	m_CheckerboardTexture = CraftNow::Texture2D::Create("assets/textures/yinghai_alpha.png");
	m_Tail_mapTexture = CraftNow::Texture2D::Create("assets/textures/tilemap_packed.png");

	m_Sub1 = CraftNow::SubTexture2D::CreateFromCoords(m_Tail_mapTexture, { 5, 10 }, { 16,16 }, { 1,1 });
	m_Sub2 = CraftNow::SubTexture2D::CreateFromCoords(m_Tail_mapTexture, { 4, 9 }, { 16,16 }, { 1,2 });

}

void Sandbox2D::OnDetach()
{
	CN_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(CraftNow::Timestep ts)
{
	CN_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	CraftNow::Renderer2D::ResetStats();

	{
		CN_PROFILE_SCOPE("Renderer Prep");
		CraftNow::RenderCommand::SetClearColor({ 0.27f, 0.447f, 0.682f, 1.0f });
		CraftNow::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		CN_PROFILE_SCOPE("Renderer Draw");

		CraftNow::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//CraftNow::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		//CraftNow::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		//画纹理阵列，设置深度靠后
		CraftNow::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
		//旋转纹理阵列
		CraftNow::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
		CraftNow::Renderer2D::EndScene();

		CraftNow::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//画可变颜色的长方形，但透明通道存在问题,
		//透明通道需要按顺序执行？最上层的渲染代码应该放最后执行
		CraftNow::Renderer2D::DrawQuad({ 0.5f, -0.0f , 0.1f}, { 0.5f, 0.75f }, m_SquareColor);
		//CraftNow::Renderer2D::EndScene();

		//CraftNow::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				CraftNow::Renderer2D::DrawQuad({ x, y ,0.2f }, { 0.45f, 0.45f }, color);
			}
		}
		CraftNow::Renderer2D::EndScene();


		//------------testing subtexture-------------
		//CraftNow::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//CraftNow::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_Tail_mapTexture);
		////CraftNow::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_Sub1);
		////CraftNow::Renderer2D::DrawQuad({ 2.0f, 0.0f }, { 1.0f, 2.0f }, m_Sub2);
		//CraftNow::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	CN_PROFILE_FUNCTION();
	
	ImGui::Begin("Settings");
	auto stats = CraftNow::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
	

}

void Sandbox2D::OnEvent(CraftNow::Event& e)
{
	m_CameraController.OnEvent(e);
}