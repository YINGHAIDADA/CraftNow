#pragma once
#include "CraftNow/Core/Log.h"


#ifdef CN_DEBUG
	#if defined(CN_PLATFORM_WINDOWS)
		#define CN_DEBUGBREAK() __debugbreak()
	#elif defined(CN_PLATFORM_LINUX)
		#include <signal.h>
		#define CN_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
		#define CN_ENABLE_ASSERTS
#else
	#define CN_DEBUGBREAK()
#endif

#ifdef CN_ENABLE_ASSERTS
	#define CN_ASSERT(x, ...) { if(!(x)) { CN_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); } }
	#define CN_CORE_ASSERT(x, ...) { if(!(x)) { CN_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); } }
#else
	#define CN_ASSERT(x, ...)
	#define CN_CORE_ASSERT(x, ...)
#endif // 


#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)