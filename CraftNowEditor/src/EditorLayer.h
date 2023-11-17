#pragma once

#include "CraftNow.h"
#include "Panels/SceneHierarchyPanel.h"

namespace CraftNow {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		OrthographicCameraController m_CameraController;

		// ----------Temp----------------------
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_Tail_mapTexture;
		Ref<SubTexture2D> m_Sub1, m_Sub2;

		Ref<Framebuffer> m_Framebuffer;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = true;

		glm::vec4 m_SquareColor = { 0.0f, 0.8f, 0.0f, 0.7f };
		//-----------------------------------


		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		//Scope<ContentBrowserPanel> m_ContentBrowserPanel;
	};
}
