#pragma once

#include "CraftNow.h"

class ExampleLayer : public CraftNow::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	void OnUpdate(CraftNow::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(CraftNow::Event& evrnt) override;
private:
	CraftNow::ShaderLibrary m_ShaderLibrary;

	CraftNow::Ref<CraftNow::VertexArray> m_VertexArray;
	CraftNow::Ref<CraftNow::VertexArray> m_SquareVA;
	CraftNow::Ref<CraftNow::VertexArray> m_TextureVA;

	CraftNow::Ref<CraftNow::Texture2D> m_Texture;

	CraftNow::OrthographicCameraController m_CameraController;

	glm::vec3 m_SQColor = { 0.8f, 0.8f, 0.8f };
};