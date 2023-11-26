#pragma once

#include "CraftNow.h"
#include "Panels/SceneHierarchyPanel.h"

#include "CraftNow/Renderer/EditorCamera.h"

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
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnDuplicateEntity();

		// For custom titlebars
		void UI_DrawTitlebar(float& outTitlebarHeight);
		void UI_DrawMenubar();
	private:
		OrthographicCameraController m_CameraController;

		// ----------Temp----------------------
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_Tail_mapTexture;
		Ref<SubTexture2D> m_Sub1, m_Sub2;


		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = true;

		glm::vec4 m_SquareColor = { 0.0f, 0.8f, 0.0f, 0.7f };
		//-----------------------------------

		int m_GizmoType = -1;

		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;


		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

		EditorCamera m_EditorCamera;

		Entity m_HoveredEntity;


		//TODO: 图标资源, 按理来说应该为静态，后续如果为运行时单独创建Application，需要考虑移出
		Ref<Texture> m_AppHeaderIcon;
		Ref<Texture> m_IconClose;
		Ref<Texture> m_IconMinimize;
		Ref<Texture> m_IconMaximize;
		Ref<Texture> m_IconRestore;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		//Scope<ContentBrowserPanel> m_ContentBrowserPanel;
	};
}
