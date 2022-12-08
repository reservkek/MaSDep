#pragma once

#ifdef MSD_PLATFORM_WINDOWS
	#ifdef MSD_BUILD_DLL
		#define MSD_API __declspec(dllexport)
	#else
		#define MSD_API __declspec(dllimport)
	#endif
#endif