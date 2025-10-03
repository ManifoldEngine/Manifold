#include "Logger.h"

#include <Core/Containers/Map.h>
#include <Core/TimeSystem.h>

#include <iostream>
#include <fstream>
#include <mutex>

using namespace Mani;

namespace LogColors
{
	const std::string RESET = "\033[0m";
	const std::string BLACK = "\033[30m";			/* Black */
	const std::string RED = "\033[31m";			/* Red */
	const std::string GREEN = "\033[32m";			/* Green */
	const std::string YELLOW = "\033[33m";			/* Yellow */
	const std::string BLUE = "\033[34m";			/* Blue */
	const std::string MAGENTA = "\033[35m";			/* Magenta */
	const std::string CYAN = "\033[36m";			/* Cyan */
	const std::string WHITE = "\033[37m";			/* White */
	const std::string BOLDBLACK = "\033[1m\033[30m";	/* Bold Black */
	const std::string BOLDRED = "\033[1m\033[31m";	/* Bold Red */
	const std::string BOLDGREEN = "\033[1m\033[32m";	/* Bold Green */
	const std::string BOLDYELLOW = "\033[1m\033[33m";	/* Bold Yellow */
	const std::string BOLDBLUE = "\033[1m\033[34m";	/* Bold Blue */
	const std::string BOLDMAGENTA = "\033[1m\033[35m";	/* Bold Magenta */
	const std::string BOLDCYAN = "\033[1m\033[36m";	/* Bold Cyan */
	const std::string BOLDWHITE = "\033[1m\033[37m";	/* Bold White */
}

const Map<ELogLevel, std::string_view> LEVEL_TO_COLOR_MAP =
{
	{ ELogLevel::Verbose, LogColors::BLUE },
	{ ELogLevel::Log, LogColors::WHITE },
	{ ELogLevel::Warning, LogColors::YELLOW },
	{ ELogLevel::Error, LogColors::RED },
};

void logToStream(const std::string_view& channel, ELogLevel level, const std::string_view& log, std::ostream& stream)
{
#if MANI_CONSOLE_APP
	std::cout << LEVEL_TO_COLOR_MAP.get(level) << "[" << TimeSystem::getTimeFormatted() << "]" << "[" << channel << "]: " << log << LogColors::RESET << std::endl;
#else
#if MANI_WINDOWS
	std::stringstream ss;
	ss << LEVEL_TO_COLOR_MAP[level];
	ss << "[" << TimeSystem::getTimeFormatted() << "]" << "[" << channel << "]: " << log;
	ss << LogColors::RESET << "\n";
	std::string s = ss.str();
	std::wstring ws(s.begin(), s.end());
	OutputDebugStringW(ws.c_str());
#endif
#endif
}

struct Logger::State
{
	std::mutex mutex;
	std::mutex fileMutex;
	Map<std::string_view, ELogLevel> channels;
	std::ofstream fileStream;
	bool isSuppressed = false;
};

void Logger::log(const std::string_view& channel, ELogLevel level, const std::string_view& log)
{
	if (!m_state->isSuppressed)
	{
		std::lock_guard<std::mutex> lock(m_state->mutex);
		ELogLevel channelLevel = m_state->channels.getOrAdd(channel, ELogLevel::Log);
		if (level > ELogLevel::Disabled && level >= channelLevel)
		{
			logToStream(channel, level, log, std::cout);
		}
	}

	if (m_state->fileStream.is_open())
	{
		std::lock_guard<std::mutex> lock(m_state->fileMutex);
		logToStream(channel, level, log, m_state->fileStream);
	}
}

void Logger::setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel)
{
	std::lock_guard<std::mutex> lock(m_state->mutex);
	m_state->channels[channel] = logLevel;
}

void Mani::Logger::setFile(const std::filesystem::path& path)
{
	std::lock_guard<std::mutex> lock(m_state->fileMutex);
	if (m_state->fileStream.is_open())
	{
		m_state->fileStream.close();
	}
	
	m_state->fileStream.open(path, std::ios::app);
	
	if (!m_state->fileStream.is_open())
	{
		log(Log, ELogLevel::Error, std::format("Failed to open log file at {}", path.string()));
	}
}

void Logger::suppress()
{
	std::lock_guard<std::mutex> lock(m_state->mutex);
	m_state->isSuppressed = true;
}

void Logger::unsuppress()
{
	std::lock_guard<std::mutex> lock(m_state->mutex);
	m_state->isSuppressed = false;
}

Logger::Logger()
{
	m_state = std::make_unique<State>();
}

Logger::~Logger()
{
	if (m_state->fileStream.is_open())
	{
		m_state->fileStream.close();
	}
}
