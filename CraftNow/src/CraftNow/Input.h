#pragma once

#include "CraftNow/KeyCodes.h"
#include "CraftNow/MouseCodes.h"

#include <glm/glm.hpp>

namespace CraftNow {

	class CRAFTNOW_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}