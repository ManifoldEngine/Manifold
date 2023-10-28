#include "Log.h"
#include <Core/CoreTime.h>
#include <iostream>
#include <sstream>

using namespace ECSEngine;

#define RESET		"\033[0m"
#define BLACK		"\033[30m"			/* Black */
#define RED			"\033[31m"			/* Red */
#define GREEN		"\033[32m"			/* Green */
#define YELLOW		"\033[33m"			/* Yellow */
#define BLUE		"\033[34m"			/* Blue */
#define MAGENTA		"\033[35m"			/* Magenta */
#define CYAN		"\033[36m"			/* Cyan */
#define WHITE		"\033[37m"			/* White */
#define BOLDBLACK   "\033[1m\033[30m"	/* Bold Black */
#define BOLDRED     "\033[1m\033[31m"	/* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"	/* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"	/* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"	/* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"	/* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"	/* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"	/* Bold White */

std::unordered_map<ELogLevel, std::string_view> LEVEL_TO_COLOR_MAP = {
	{ ELogLevel::Verbose, BLUE },
	{ ELogLevel::Log, WHITE },
	{ ELogLevel::Warning, YELLOW },
	{ ELogLevel::Error, RED },
};

void LogSystem::log(const std::string_view& channel, ELogLevel level, const std::string_view& log)
{
	if (!channels.contains(channel))
	{
		channels[channel] = ELogLevel::Log;
	}

	if (level > ELogLevel::Disabled && level >= channels[channel])
	{
		std::stringstream ss;
		ss << LEVEL_TO_COLOR_MAP[level];
		ss << "[" << Time::getTimeFormatted() << "]" << "[" << channel << "]: " << log;
		ss << RESET;
		std::cout << ss.str() << std::endl;
	}
}

void LogSystem::setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel)
{
	channels[channel] = logLevel;
}
