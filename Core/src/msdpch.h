#pragma once

#include <stdlib.h>
#include <memory>
#include <functional>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <utility>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#ifdef MSD_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#define IMPLOT_DISABLE_OBSOLETE_FUNCTIONS