#pragma once

#include "CraftNow/Core/PlatformDetection.h"

#include <memory>

#if defined(CN_PLATFORM_WINDOWS)
#define CN_DEBUGBREAK() __debugbreak()
#elif defined(CN_PLATFORM_LINUX)
#include <signal.h>
#define CN_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#ifdef CN_DEBUG
	#define CN_ENABLE_ASSERTS
#endif

#ifndef CN_DIST
	#define CN_ENABLE_VERIFY
#endif

#define CN_EXPAND_MACRO(x) x
#define CN_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

//#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace CraftNow {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "CraftNow/Core/Log.h"
#include "CraftNow/Core/Assert.h"
