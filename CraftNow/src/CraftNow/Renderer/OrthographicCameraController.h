#pragma once

#include "CraftNow/Renderer/OrthographicCamera.h"
#include "CraftNow/Core/Timestep.h"

#include "CraftNow/Events/ApplicationEvent.h"
#include "CraftNow/Events/MouseEvent.h"

namespace CraftNow {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_LastCameraPosition = { 0.0f, 0.0f, 0.0f };

		float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;

		glm::vec2 m_lastMousePosition = { 0.0f, 0.0f };

		float m_mouseScale = 1.0f;
	};

}