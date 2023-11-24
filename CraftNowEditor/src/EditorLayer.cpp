﻿#include "EditorLayer.h"
#include "CraftNow/Scene/SceneSerializer.h"
#include "CraftNow/Math/Math.h"
//#include "CraftNow/Scripting/ScriptEngine.h"
//#include "CraftNow/Renderer/Font.h"
#include "CraftNow/Utils/PlatformUtils.h"

//#include "CraftNow/Utils/ChineseUtils.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGuizmo.h"

#include "UI/UI.h"

namespace CraftNow {

	#include "CraftNow/Embed/Icon.embed"
	#include "CraftNow/Embed/WindowImages.embed"

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_ViewportSize({1280, 720})
	{
	}

	void EditorLayer::OnAttach()
	{
		CN_PROFILE_FUNCTION();

		m_CheckerboardTexture = Texture2D::Create("assets/textures/yinghai_alpha.png");
		m_Tail_mapTexture = Texture2D::Create("../Sandbox/assets/textures/Legend_of_Zelda/maps/Legend_of_Zelda.png");

		m_Sub1 = SubTexture2D::CreateFromCoords(m_Tail_mapTexture, { 5, 10 }, { 16,16 }, { 1,1 });
		m_Sub2 = SubTexture2D::CreateFromCoords(m_Tail_mapTexture, { 4, 9 }, { 16,16 }, { 1,2 });

		//Framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);


		//加载图标
		{
			uint32_t w, h;
			void* data = Texture2D::Decode(g_Icon, sizeof(g_Icon), w, h);
			m_AppHeaderIcon = Texture2D::Create({ w,h,ImageFormat::RGBA8 ,true });
			m_AppHeaderIcon->SetData(data, w * h * 4);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Texture2D::Decode(g_WindowMinimizeIcon, sizeof(g_WindowMinimizeIcon), w, h);
			m_IconMinimize = Texture2D::Create({ w,h,ImageFormat::RGBA8 ,true });
			m_IconMinimize->SetData(data, w * h * 4);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Texture2D::Decode(g_WindowMaximizeIcon, sizeof(g_WindowMaximizeIcon), w, h);
			m_IconMaximize = Texture2D::Create({ w,h,ImageFormat::RGBA8 ,true });
			m_IconMaximize->SetData(data, w * h * 4);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Texture2D::Decode(g_WindowRestoreIcon, sizeof(g_WindowRestoreIcon), w, h);
			m_IconRestore = Texture2D::Create({ w,h,ImageFormat::RGBA8 ,true });
			m_IconRestore->SetData(data, w * h * 4);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Texture2D::Decode(g_WindowCloseIcon, sizeof(g_WindowCloseIcon), w, h);
			m_IconClose = Texture2D::Create({ w,h,ImageFormat::RGBA8 ,true });
			m_IconClose->SetData(data, w * h * 4);
			free(data);
		}

		//------Scene---------

		//m_ActiveScene = CreateRef<Scene>();

		#if 0

		auto yinghai = m_ActiveScene->CreateEntity("YINGHAI");
		yinghai.AddComponent<SpriteRendererComponent>(m_CheckerboardTexture);

		auto redSquare = m_ActiveScene->CreateEntity("red");
		redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 0.6f });

		m_SquareEntity = m_ActiveScene->CreateEntity("Map", { 4.96f, 3.84f, -0.1f }, { 0.0f, 0.0f, 0.0f }, { 49.92f, 31.68f, 1.0f });
		m_SquareEntity.AddComponent<SpriteRendererComponent>(m_Tail_mapTexture);

		m_CameraEntity = m_ActiveScene->CreateEntity(u8"相机A");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity(u8"相机B");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				translation[0] = rand() % 10 - 5.0f;
			}

			void OnDestroy()
			{
			}
			void OnUpdate(Timestep ts)
			{
				auto& translation = GetComponent<TransformComponent>().Translation;

				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::A))
					translation[0] -= speed * ts;
				if (Input::IsKeyPressed(Key::D))
					translation[0] += speed * ts;
				if (Input::IsKeyPressed(Key::W))
					translation[1] += speed * ts;
				if (Input::IsKeyPressed(Key::S))
					translation[1] -= speed * ts;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		#endif

		//Panel
		//m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		CN_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		CN_PROFILE_FUNCTION();

		//Viewport Resize
		{
			FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
				(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
			{
				m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

				//场景相机更新
				if(m_ActiveScene)
					m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			}
		}

		// Update
		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.27f, 0.447f, 0.682f, 1.0f });
		RenderCommand::Clear();
		
		#if 0
		{
			static float rotation = 0.0f;
			rotation += ts * 50.0f;

			CN_PROFILE_SCOPE("Renderer Draw");

			//Renderer2D::BeginScene(m_CameraController.GetCamera());
			////Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
			////Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
			////画纹理阵列，设置深度靠后
			//Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
			////旋转纹理阵列
			//Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
			//Renderer2D::EndScene();

			//Renderer2D::BeginScene(m_CameraController.GetCamera());
			////画可变颜色的长方形，但透明通道存在问题,
			////透明通道需要按顺序执行？最上层的渲染代码应该放最后执行
			//Renderer2D::DrawQuad({ 0.5f, -0.0f , 0.1f}, { 0.5f, 0.75f }, m_SquareColor);
			////Renderer2D::EndScene();

			////Renderer2D::BeginScene(m_CameraController.GetCamera());
			//for (float y = -5.0f; y < 5.0f; y += 0.5f)
			//{
			//	for (float x = -5.0f; x < 5.0f; x += 0.5f)
			//	{
			//		glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
			//		Renderer2D::DrawQuad({ x, y ,0.2f }, { 0.45f, 0.45f }, color);
			//	}
			//}
			//Renderer2D::EndScene();


			//------------testing subtexture-------------
			Renderer2D::BeginScene(m_CameraController.GetCamera());
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_Tail_mapTexture);
			//Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_Sub1);
			//Renderer2D::DrawQuad({ 2.0f, 0.0f }, { 1.0f, 2.0f }, m_Sub2);
			Renderer2D::EndScene();

		}
		#endif

		//Scene testing
		//Renderer2D::BeginScene(m_CameraController.GetCamera());
		// Update scene
		if(m_ActiveScene)
			m_ActiveScene->OnUpdate(ts);
		//Renderer2D::EndScene();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		CN_PROFILE_FUNCTION();

		
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			if (!Application::Get().GetSpecification().CustomTitlebar)
				window_flags |= ImGuiWindowFlags_MenuBar;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

		const bool isMaximized = Application::Get().IsMaximized();

		//old
		#if 0
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);
		}
		#else
		//new
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);

			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
			ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
			ImGui::PopStyleColor(); // MenuBarBg
			ImGui::PopStyleVar(2);

			ImGui::PopStyleVar(2);
		}
		#endif

		#if 1

		{
			ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
			// Draw window border if the window is not maximized
			if (!isMaximized)
				UI::RenderWindowOuterBorders(ImGui::GetCurrentWindow());

			ImGui::PopStyleColor(); // ImGuiCol_Border
		}
		#endif

		if (Application::Get().GetSpecification().CustomTitlebar)
		{
			float titleBarHeight;
			UI_DrawTitlebar(titleBarHeight);
			ImGui::SetCursorPosY(titleBarHeight);

		}

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		//设置最小宽度 
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 350.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;


		if (!Application::Get().GetSpecification().CustomTitlebar)
			UI_DrawMenubar();
		//标题栏菜单
		//if (ImGui::BeginMenuBar())
		//{
		//	if (ImGui::BeginMenu(u8"文件"))
		//	{
		//		// Disabling fullscreen would allow the window to be moved to the front of other windows, 
		//		// which we can't undo at the moment without finer window depth/z control.
		//		//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
		//		if (ImGui::MenuItem(u8"打开工程...", "Ctrl+O"))
		//			OpenProject();

		//		ImGui::Separator();

		//		if (ImGui::MenuItem(u8"新 场景", "Ctrl+N"))
		//			NewScene();

		//		if (ImGui::MenuItem(u8"打开 场景"))
		//			OpenScene();

		//		if (ImGui::MenuItem(u8"保存 场景", "Ctrl+S"))
		//			SaveScene();

		//		if (ImGui::MenuItem(u8"保存 场景 到...", "Ctrl+Shift+S"))
		//			SaveSceneAs();

		//		ImGui::Separator();

		//		/*if (ImGui::MenuItem(u8"序列化"))
		//		{
		//			SceneSerializer serializer(m_ActiveScene);
		//			serializer.Serialize("assets/scenes/AlphaTest.craft");
		//		}

		//		if (ImGui::MenuItem(u8"解析序列"))
		//		{
		//			SceneSerializer serializer(m_ActiveScene);
		//			serializer.Deserialize("assets/scenes/AlphaTest.craft");
		//		}*/


		//		if (ImGui::MenuItem(u8"退出")) Application::Get().Close();
		//		ImGui::EndMenu();
		//	}

		//	if (ImGui::BeginMenu(u8"脚本"))
		//	{
		//		if (ImGui::MenuItem(u8"重新加载部件", "Ctrl+R"))
		//		{
		//			//ScriptEngine::ReloadAssembly();
		//		}

		//		ImGui::EndMenu();
		//	}

		//	ImGui::EndMenuBar();
		//}

		//Panel
		m_SceneHierarchyPanel.OnImGuiRender();


		{
			ImGui::Begin(u8"状态");
			auto stats = Renderer2D::GetStats();
			ImGui::Text(u8"Renderer2D 状态:");
			ImGui::Text(u8"Draw Calls: %d", stats.DrawCalls);
			ImGui::Text(u8"Quads: %d", stats.QuadCount);
			ImGui::Text(u8"Vertices: %d", stats.GetTotalVertexCount());
			ImGui::Text(u8"Indices: %d", stats.GetTotalIndexCount());

			/*ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));*/

			//---------------SceneCamera-------------

			/*if (ImGui::Checkbox(u8"相机 A", &m_PrimaryCamera))
			{
				m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
				m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
			}*/

			//---------------------------------
			ImGui::End();
		}

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
			ImGui::Begin(u8"视窗");


			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x,m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });

			// Gizmos
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				// Camera
				auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
				const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
				const glm::mat4& cameraProjection = camera.GetProjection();
				glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

				// Entity transform
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();

				// Snapping
				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = 0.5f; // Snap to 0.5m for translation/scale
				// Snap to 45 degrees for rotation
				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					glm::vec3 deltaRotation = rotation - tc.Rotation;
					tc.Translation = translation;
					tc.Rotation += deltaRotation;
					tc.Scale = scale;
				}
			}


			ImGui::End();
			ImGui::PopStyleVar();
		}

		ImGui::End();

	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		if (m_SceneState == SceneState::Edit)
		{
			//m_EditorCamera.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				OpenProject();

			break;
		}
		case Key::S:
		{
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}

			break;
		}

		// Scene Commands
		case Key::D:
		{
			if (control)
				OnDuplicateEntity();

			break;
		}

		// Gizmos
		case Key::Q:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = -1;
			break;
		}
		case Key::W:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::R:
		{
			if (control)
			{
				//ScriptEngine::ReloadAssembly();
			}
			else
			{
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
			break;
		}
		case Key::Delete:
		{
			if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
			{
				Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
				if (selectedEntity)
				{
					m_SceneHierarchyPanel.SetSelectedEntity({});
					m_ActiveScene->DestroyEntity(selectedEntity);
				}
			}
			break;
		}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		/*if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}*/
		return false;
	}

	void EditorLayer::NewProject()
	{
		Project::New();
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("CraftNow Project (*.cnproj)\0*.cnproj\0");
		if (filepath.empty())
			return false;

		OpenProject(filepath);
		return true;
	}


	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		/*if (Project::Load(path))
		{
			ScriptEngine::Init();

			auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath);
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();

		}*/
	}

	void EditorLayer::SaveProject()
	{
		// Project::SaveActive();
	}

	void EditorLayer::NewScene()
	{
		//TODO: 如果当前存在场景，提醒保存当前场景或者丢弃

		//TODO: 可能存在内存泄露，应该在某处处理
		m_ActiveScene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("CraftNow Scene (*.craft)\0*.craft\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			//OnSceneStop();

		if (path.extension().string() != ".craft")
		{
			CN_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		//TODO: 可能存在内存泄露，应该在某处处理
		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;

			//修复打开场景不显示的问题
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("CraftNow Scene (*.craft)\0*.craft\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}


	void EditorLayer::OnDuplicateEntity()
	{

	}

	void EditorLayer::UI_DrawTitlebar(float& outTitlebarHeight)
	{
		auto& instance = Application::Get();
		auto& specification = instance.GetSpecification();

		const float titlebarHeight = 58.0f;
		const bool isMaximized = instance.IsMaximized();
		float titlebarVerticalOffset = isMaximized ? -6.0f : 0.0f;
		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset));
		const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + titlebarHeight };
		auto* bgDrawList = ImGui::GetBackgroundDrawList();
		auto* fgDrawList = ImGui::GetForegroundDrawList();
		bgDrawList->AddRectFilled(titlebarMin, titlebarMax, UI::Colors::Theme::titlebar);
		// DEBUG TITLEBAR BOUNDS
		 fgDrawList->AddRect(titlebarMin, titlebarMax, UI::Colors::Theme::invalidPrefab);

		// Logo
		{
			const int logoWidth = 48;// m_LogoTex->GetWidth();
			const int logoHeight = 48;// m_LogoTex->GetHeight();
			const ImVec2 logoOffset(16.0f + windowPadding.x, 5.0f + windowPadding.y + titlebarVerticalOffset);
			const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
			fgDrawList->AddImage((ImTextureID)(intptr_t)m_AppHeaderIcon->GetRendererID(), logoRectStart, logoRectMax);
		}

		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

		static float moveOffsetX;
		static float moveOffsetY;
		const float w = ImGui::GetContentRegionAvail().x;
		const float buttonsAreaWidth = 94;

		// Title bar drag area
		// On Windows we hook into the GLFW win32 window internals
		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset)); // Reset cursor pos
		// DEBUG DRAG BOUNDS
		 fgDrawList->AddRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + w - buttonsAreaWidth, ImGui::GetCursorScreenPos().y + titlebarHeight), UI::Colors::Theme::invalidPrefab);
		ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));

		instance.SetTitleBarHovered(ImGui::IsItemHovered());

		if (instance.IsMaximized())
		{
			float windowMousePosY = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
			if (windowMousePosY >= 0.0f && windowMousePosY <= 5.0f)
				instance.SetTitleBarHovered(true); // Account for the top-most pixels which don't register
		}

		// Draw Menubar
		//if (m_MenubarCallback)
		//{
		ImGui::SuspendLayout();
		{
			ImGui::SetItemAllowOverlap();
			const float logoHorizontalOffset = 16.0f * 2.0f + 48.0f + windowPadding.x;
			ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, 6.0f + titlebarVerticalOffset));
			UI_DrawMenubar();

			if (ImGui::IsItemHovered())
				instance.SetTitleBarHovered(false);
		}

		ImGui::ResumeLayout();
		//}

		{
			// Centered Window title
			ImVec2 currentCursorPos = ImGui::GetCursorPos();
			ImVec2 textSize = ImGui::CalcTextSize(specification.Name.c_str());
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f, 2.0f + windowPadding.y + 6.0f));
			ImGui::Text("%s", specification.Name.c_str()); // Draw title
			ImGui::SetCursorPos(currentCursorPos);
		}

		// Window buttons
		const ImU32 buttonColN = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 0.9f);
		const ImU32 buttonColH = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.2f);
		const ImU32 buttonColP = UI::Colors::Theme::textDarker;
		const float buttonWidth = 14.0f;
		const float buttonHeight = 14.0f;

		// Minimize Button

		ImGui::Spring();
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = m_IconMinimize->GetWidth();
			const int iconHeight = m_IconMinimize->GetHeight();
			const float padY = (buttonHeight - (float)iconHeight) / 2.0f;
			if (ImGui::InvisibleButton("Minimize", ImVec2(buttonWidth, buttonHeight)))
			{
				// TODO: move this stuff to a better place, like Window class
				/*if (m_WindowHandle)
				{
					Application::Get().QueueEvent([windowHandle = m_WindowHandle]() { glfwIconifyWindow(windowHandle); });
				}*/
			}

			UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));
		}


		// Maximize Button
		ImGui::Spring(-1.0f, 17.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = m_IconMaximize->GetWidth();
			const int iconHeight = m_IconMaximize->GetHeight();

			const bool isMaximized = instance.IsMaximized();

			if (ImGui::InvisibleButton("Maximize", ImVec2(buttonWidth, buttonHeight)))
			{
				/*Application::Get().QueueEvent([isMaximized, windowHandle = m_WindowHandle]()
					{
						if (isMaximized)
							glfwRestoreWindow(windowHandle);
						else
							glfwMaximizeWindow(windowHandle);
					});*/
			}

			UI::DrawButtonImage(isMaximized ? m_IconRestore : m_IconMaximize, buttonColN, buttonColH, buttonColP);
		}

		// Close Button
		ImGui::Spring(-1.0f, 15.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = m_IconClose->GetWidth();
			const int iconHeight = m_IconClose->GetHeight();
			if (ImGui::InvisibleButton("Close", ImVec2(buttonWidth, buttonHeight)))
				Application::Get().Close();

			UI::DrawButtonImage(m_IconClose, UI::Colors::Theme::text, UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.4f), buttonColP);
		}

		ImGui::Spring(-1.0f, 18.0f);
		ImGui::EndHorizontal();

		outTitlebarHeight = titlebarHeight;
	}

	void EditorLayer::UI_DrawMenubar()
	{
		#if 1
		if (Application::Get().GetSpecification().CustomTitlebar)
		{
			const ImRect menuBarRect = { ImGui::GetCursorPos(), { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() } };

			ImGui::BeginGroup();
			if (UI::BeginMenubar(menuBarRect))
			{
				//m_MenubarCallback();
				if (ImGui::BeginMenu(u8"文件"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows, 
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
					if (ImGui::MenuItem(u8"打开工程...", "Ctrl+O"))
						OpenProject();

					ImGui::Separator();

					if (ImGui::MenuItem(u8"新 场景", "Ctrl+N"))
						NewScene();

					if (ImGui::MenuItem(u8"打开 场景"))
						OpenScene();

					if (ImGui::MenuItem(u8"保存 场景", "Ctrl+S"))
						SaveScene();

					if (ImGui::MenuItem(u8"保存 场景 到...", "Ctrl+Shift+S"))
						SaveSceneAs();

					ImGui::Separator();

					/*if (ImGui::MenuItem(u8"序列化"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Serialize("assets/scenes/AlphaTest.craft");
					}

					if (ImGui::MenuItem(u8"解析序列"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Deserialize("assets/scenes/AlphaTest.craft");
					}*/


					if (ImGui::MenuItem(u8"退出")) Application::Get().Close();
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(u8"脚本"))
				{
					if (ImGui::MenuItem(u8"重新加载部件", "Ctrl+R"))
					{
						//ScriptEngine::ReloadAssembly();
					}

					ImGui::EndMenu();
				}

				UI::EndMenubar();
			}

			ImGui::EndGroup();

		}
		else
		{
			if (ImGui::BeginMenuBar())
			{
				//m_MenubarCallback();
				if (ImGui::BeginMenu(u8"文件"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows, 
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
					if (ImGui::MenuItem(u8"打开工程...", "Ctrl+O"))
						OpenProject();

					ImGui::Separator();

					if (ImGui::MenuItem(u8"新 场景", "Ctrl+N"))
						NewScene();

					if (ImGui::MenuItem(u8"打开 场景"))
						OpenScene();

					if (ImGui::MenuItem(u8"保存 场景", "Ctrl+S"))
						SaveScene();

					if (ImGui::MenuItem(u8"保存 场景 到...", "Ctrl+Shift+S"))
						SaveSceneAs();

					ImGui::Separator();

					/*if (ImGui::MenuItem(u8"序列化"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Serialize("assets/scenes/AlphaTest.craft");
					}

					if (ImGui::MenuItem(u8"解析序列"))
					{
						SceneSerializer serializer(m_ActiveScene);
						serializer.Deserialize("assets/scenes/AlphaTest.craft");
					}*/


					if (ImGui::MenuItem(u8"退出")) Application::Get().Close();
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(u8"脚本"))
				{
					if (ImGui::MenuItem(u8"重新加载部件", "Ctrl+R"))
					{
						//ScriptEngine::ReloadAssembly();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}
		#endif
	}
}
