#pragma once

#include "CraftNow/Layer.h"

namespace CraftNow
{

	class CRAFTNOW_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(Event &event) override;

	private:
		float m_Time = 0.0f;
	};
}