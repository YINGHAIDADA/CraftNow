#include "EditorLayer.h"
#include "CraftNow/Scene/SceneSerializer.h"
#include "CraftNow/Math/Math.h"
//#include "CraftNow/Scripting/ScriptEngine.h"
//#include "CraftNow/Renderer/Font.h"
#include "CraftNow/Utils/PlatformUtils.h"

//#include "CraftNow/Utils/ChineseUtils.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace CraftNow {


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

		//------Scene---------

		m_ActiveScene = CreateRef<Scene>();

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
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
		
		if(false)
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

		//Scene testing
		Renderer2D::BeginScene(m_CameraController.GetCamera());
		// Update scene
		m_ActiveScene->OnUpdate(ts);
		Renderer2D::EndScene();

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
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

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

		//style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
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

				if (ImGui::MenuItem(u8"保存 场景", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem(u8"保存 场景 到...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem(u8"序列化"))
				{
					SceneSerializer serializer(m_ActiveScene);
					serializer.Serialize("assets/scenes/AlphaTest.craft");
				}

				if (ImGui::MenuItem(u8"解析序列"))
				{
					SceneSerializer serializer(m_ActiveScene);
					serializer.Deserialize("assets/scenes/AlphaTest.craft");
				}


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

		//Panel
		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin(u8"状态");
		auto stats = Renderer2D::GetStats();
		ImGui::Text(u8"Renderer2D 状态:");
		ImGui::Text(u8"Draw Calls: %d", stats.DrawCalls);
		ImGui::Text(u8"Quads: %d", stats.QuadCount);
		ImGui::Text(u8"Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text(u8"Indices: %d", stats.GetTotalIndexCount());

		/*ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));*/

		//---------------SceneCamera-------------

		if (ImGui::Checkbox(u8"相机 A", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		}

		//---------------------------------
		ImGui::End();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin(u8"视窗");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		
		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x,m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 } );
		ImGui::End();
		ImGui::PopStyleVar();

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
		/*case Key::Q:
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
				ScriptEngine::ReloadAssembly();
			}
			else
			{
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
			break;
		}*/
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

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
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
		std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.craft)\0*.craft\0");
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

}
