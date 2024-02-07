#include "msdpch.h"

export module MSDLog;

namespace MSD
{
	enum LogType : int
	{
		MSD_LOG_ERROR,
		MSD_LOG_MESSAGE,
		MSD_LOG_WARNING,
		MSD_LOG_SYSTEM
	};

	export std::vector<std::pair<LogType, std::string>> LogData;

}