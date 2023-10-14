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
