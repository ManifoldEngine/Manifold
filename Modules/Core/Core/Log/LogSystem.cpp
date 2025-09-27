#include "Log.h"
#include <Core/TimeSystem.h>
#include <iostream>
#include <sstream>
#include <unordered_map>

#if MANI_WINDOWS && !MANI_CONSOLE_APP
#include <windows.h>
#include <debugapi.h>
#endif

using namespace Mani;

namespace LogColors
{
	const std::string RESET			= "\033[0m";
	const std::string BLACK			= "\033[30m";			/* Black */
	const std::string RED			= "\033[31m";			/* Red */
	const std::string GREEN			= "\033[32m";			/* Green */
	const std::string YELLOW		= "\033[33m";			/* Yellow */
	const std::string BLUE			= "\033[34m";			/* Blue */
	const std::string MAGENTA		= "\033[35m";			/* Magenta */
	const std::string CYAN			= "\033[36m";			/* Cyan */
	const std::string WHITE			= "\033[37m";			/* White */
	const std::string BOLDBLACK		= "\033[1m\033[30m";	/* Bold Black */
	const std::string BOLDRED		= "\033[1m\033[31m";	/* Bold Red */
	const std::string BOLDGREEN		= "\033[1m\033[32m";	/* Bold Green */
	const std::string BOLDYELLOW	= "\033[1m\033[33m";	/* Bold Yellow */
	const std::string BOLDBLUE		= "\033[1m\033[34m";	/* Bold Blue */
	const std::string BOLDMAGENTA	= "\033[1m\033[35m";	/* Bold Magenta */
	const std::string BOLDCYAN		= "\033[1m\033[36m";	/* Bold Cyan */
	const std::string BOLDWHITE		= "\033[1m\033[37m";	/* Bold White */
}

std::unordered_map<ELogLevel, std::string_view> LEVEL_TO_COLOR_MAP = 
{
	{ ELogLevel::Verbose, LogColors::BLUE },
	{ ELogLevel::Log, LogColors::WHITE },
	{ ELogLevel::Warning, LogColors::YELLOW },
	{ ELogLevel::Error, LogColors::RED },
};

LogSystem* LogSystem::s_logSystem = nullptr;

LogSystem::LogSystem()
{
	s_logSystem = this;
}

LogSystem::~LogSystem()
{
	s_logSystem = nullptr;
}

void LogSystem::log(const std::string_view& channel, ELogLevel level, const std::string_view& log)
{
	if (!channels.contains(channel))
	{
		channels[channel] = ELogLevel::Log;
	}

	if (level > ELogLevel::Disabled && level >= channels[channel])
	{
		logImpl(channel, level, log);
	}
}

void LogSystem::s_log(const std::string_view& channel, ELogLevel level, const std::string_view& log)
{
	if (s_logSystem != nullptr)
	{
		s_logSystem->log(channel, level, log);
		return;
	}

	std::string modifiedLog = "[static] ";
	modifiedLog += log;
	logImpl(channel, level, modifiedLog);
}

void LogSystem::setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel)
{
	channels[channel] = logLevel;
}

void LogSystem::logImpl(const std::string_view& channel, ELogLevel level, const std::string_view& log)
{
	if (s_isSuppressed)
	{
		return;
	}

#if MANI_CONSOLE_APP
	std::cout << LEVEL_TO_COLOR_MAP[level] << "[" << TimeSystem::getTimeFormatted() << "]" << "[" << channel << "]: " << log << LogColors::RESET << std::endl;
#else
#if MANI_WINDOWS
	std::stringstream ss;
	ss << LEVEL_TO_COLOR_MAP[level];
	ss << "[" << TimeSystem::getTimeFormatted() << "]" << "[" << channel << "]: " << log;
	ss << RESET << "\n";
	std::string s = ss.str();
	std::wstring ws(s.begin(), s.end());
	OutputDebugStringW(ws.c_str());
#endif
#endif
}

void Mani::LogSystem::s_setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel)
{
	if (s_logSystem != nullptr)
	{
		s_logSystem->setChannelLogLevel(channel, logLevel);
	}
	else
	{
		MANI_LOG_WARNING(LogCore, "LogSystem::setChannelLogLevel called without an application context. {}'s log level won't change.", channel);
	}
}

void Mani::LogSystem::s_suppress()
{
	s_isSuppressed = true;
}

void Mani::LogSystem::s_unsuppress()
{
	s_isSuppressed = false;
}
