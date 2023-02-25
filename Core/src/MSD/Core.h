#pragma once

#include "msdpch.h"
#include "../glm/glm.hpp"

#define MSD_API
#ifdef MSD_PLATFORM_WINDOWS
#ifdef MSD_DYNAMIC_LINKING
	#ifdef MSD_BUILD_DLL
		#define MSD_API __declspec(dllexport)
	#else
		#define MSD_API __declspec(dllimport)
	#endif
#else
	#define MSD_API
#endif
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define BIND_EVENT_FN_STATIC(fn) std::bind(&fn, std::placeholders::_1)

#define ASSERT(x) if (!(x)) __debugbreak();