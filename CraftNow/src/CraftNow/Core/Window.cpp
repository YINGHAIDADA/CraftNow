#include "cnpch.h"
#include "CraftNow/Core/Window.h"

#ifdef CN_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace CraftNow
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
		#ifdef CN_PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(props);
		#else
		CN_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
		#endif
	}

}


