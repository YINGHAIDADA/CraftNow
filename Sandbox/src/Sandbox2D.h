#pragma once

#include "CraftNow.h"

class Sandbox2D : public CraftNow::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(CraftNow::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(CraftNow::Event& e) override;
private:
	CraftNow::OrthographicCameraController m_CameraController;

	// Temp
	CraftNow::Ref<CraftNow::VertexArray> m_SquareVA;
	CraftNow::Ref<CraftNow::Shader> m_FlatColorShader;

	CraftNow::Ref<CraftNow::Texture2D> m_CheckerboardTexture;
	CraftNow::Ref<CraftNow::Texture2D> m_Tail_mapTexture;
	CraftNow::Ref<CraftNow::SubTexture2D> m_Sub1, m_Sub2;

	glm::vec4 m_SquareColor = { 0.0f, 0.8f, 0.0f, 0.7f };
};