#pragma once

#include "CraftNow/Core/KeyCodes.h"
#include "CraftNow/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace CraftNow {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}