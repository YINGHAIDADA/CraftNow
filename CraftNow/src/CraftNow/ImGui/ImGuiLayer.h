#pragma once

#include "CraftNow/Layer.h"

#include "CraftNow/Events/ApplicationEvent.h"
#include "CraftNow/Events/KeyEvent.h"
#include "CraftNow/Events/MouseEvent.h"

namespace CraftNow
{

	class CRAFTNOW_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}