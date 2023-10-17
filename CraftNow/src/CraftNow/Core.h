#pragma once


#ifdef CN_PLATFORM_WINDOWS
	#ifdef CN_BUILD_DLL
		#define CRAFTNOW_API __declspec(dllexport)
	#else
		#define CRAFTNOW_API __declspec(dllimport)
	#endif // CN_BUILD_DLL
#else
	#error CraftNow only support Windows!
#endif // CN_PLATFORM_WINDOWS

#ifdef CN_ENABLE_ASSERTS
	#define CN_ASSERT(x, ...) { if(!(x)) { CN_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); } }
	#define CN_CORE_ASSERT(x, ...){ if(!(x)) { CN_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); } }
#else
	#define CN_ASSERT(x, ...)
	#define CN_CORE_ASSERT(x, ...)
#endif // 


#define BIT(x) (1 << x)
