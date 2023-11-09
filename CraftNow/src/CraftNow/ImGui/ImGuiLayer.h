#pragma once

#include "CraftNow/Core/Layer.h"

#include "CraftNow/Events/ApplicationEvent.h"
#include "CraftNow/Events/KeyEvent.h"
#include "CraftNow/Events/MouseEvent.h"

namespace CraftNow
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();

		uint32_t GetActiveWidgetID() const;
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}