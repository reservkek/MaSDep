#pragma once

#include <memory>
#include <string>
#include <functional>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <algorithm>
#include <stdlib.h>


#ifdef MSD_PLATFORM_WINDOWS
	#ifdef MSD_BUILD_DLL
		#define MSD_API __declspec(dllexport)
	#else
		#define MSD_API __declspec(dllimport)
	#endif
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)