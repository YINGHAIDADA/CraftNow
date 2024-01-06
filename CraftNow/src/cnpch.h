#pragma once

#ifdef CN_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "CraftNow/Core/Core.h"
#include "CraftNow/Core/Buffer.h"

#include "CraftNow/Core/Log.h"

#include "CraftNow/Debug/Instrumentor.h"

#ifdef  CN_PLATFORM_WINDOWS
	#include <Windows.h>
#endif //  CN_PLATFORM_WINDOWS

