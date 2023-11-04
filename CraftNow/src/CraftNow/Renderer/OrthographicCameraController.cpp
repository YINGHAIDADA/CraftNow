#include "cnpch.h"
#include "CraftNow/Renderer/OrthographicCameraController.h"

#include "CraftNow/Core/Input.h"

#include <imgui.h>

namespace CraftNow {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		//CN_PROFILE_FUNCTION();

		//----------鼠标拖动画布事件---------------------
		ImGuiIO& io = ImGui::GetIO();

		if (!io.WantCaptureMouse)
		{
			if (CraftNow::Input::IsMouseButtonPressed(CraftNow::Mouse::ButtonMiddle))
			{
				auto mousePositon = CraftNow::Input::GetMousePosition();
				
				m_CameraPosition.x = m_LastCameraPosition.x - (mousePositon.x - m_lastMousePosition.x) * 0.2222f * 0.01f * m_CameraTranslationSpeed * m_mouseScale;
				m_CameraPosition.y = m_LastCameraPosition.y + (mousePositon.y - m_lastMousePosition.y) * 0.2222f * 0.01f * m_CameraTranslationSpeed * m_mouseScale;
			}
			else
			{
				m_lastMousePosition = CraftNow::Input::GetMousePosition();
				m_LastCameraPosition = m_CameraPosition;
			}
		}
		//----------鼠标拖动画布事件---------------------

		//----------键盘相机移动事件---------------------
		if (!io.WantCaptureKeyboard)
		{
			if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Left) || CraftNow::Input::IsKeyPressed(CraftNow::Key::A))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
				{
					m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
					m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
				}
				else
				{
					m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
					m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				}
			else if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Right) || CraftNow::Input::IsKeyPressed(CraftNow::Key::D))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
				{
					m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
					m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
				}
				else
				{
					m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
					m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				}

			if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Up) || CraftNow::Input::IsKeyPressed(CraftNow::Key::W))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
				{
					m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
					m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
				}
				else
				{
					m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
					m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				}
			else if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Down) || CraftNow::Input::IsKeyPressed(CraftNow::Key::S))
				if (CraftNow::Input::IsKeyPressed(CraftNow::Key::LeftShift))
				{
					m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
					m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * 0.1f * ts;
				}
				else
				{
					m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
					m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				}

			if (CraftNow::Input::IsKeyPressed(CraftNow::Key::Space) || CraftNow::Input::IsMouseButtonPressed(CraftNow::Mouse::ButtonRight))
			{
				m_CameraPosition.x = 0.0f; m_CameraPosition.y = 0.0f;
				m_CameraRotation = 0.0f;
				m_ZoomLevel = 1.0f;
				m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

			}

			if (m_Rotation)
			{
				if (Input::IsKeyPressed(Key::Q))
					m_CameraRotation += m_CameraRotationSpeed * ts;
				if (Input::IsKeyPressed(Key::E))
					m_CameraRotation -= m_CameraRotationSpeed * ts;

				if (m_CameraRotation > 180.0f)
					m_CameraRotation -= 360.0f;
				else if (m_CameraRotation <= -180.0f)
					m_CameraRotation += 360.0f;

				m_Camera.SetRotation(m_CameraRotation);
			}
		}

		//----------键盘相机移动事件---------------------
		

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		//CN_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_mouseScale = 900.0f / height;

		CN_CORE_TRACE("mouseScale{0}", m_mouseScale);

		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		//CN_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.15f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		//CN_PROFILE_FUNCTION();

		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}