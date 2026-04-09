#include "Logger.h"

#include <Core/Application.h>
#include <Core/Containers/Map.h>
#include <Core/TimeSystem.h>

#include <print>
#include <iostream>
#include <fstream>

#if !MANI_CONSOLE_APP && MANI_WINDOWS
#include <windows.h>
#endif

using namespace Mani;

namespace LogColors
{
	constexpr std::string_view RESET = "\033[0m";
	constexpr std::string_view GREY = "\033[90m";
	constexpr std::string_view BLACK = "\033[30m";
	constexpr std::string_view RED = "\033[31m";
	constexpr std::string_view GREEN = "\033[32m";
	constexpr std::string_view YELLOW = "\033[33m";
	constexpr std::string_view BLUE = "\033[34m";
	constexpr std::string_view MAGENTA = "\033[35m";
	constexpr std::string_view CYAN = "\033[36m";
	constexpr std::string_view WHITE = "\033[37m";

	constexpr std::string_view BOLDGREY = "\033[1m\033[90m";
	constexpr std::string_view BOLDBLACK = "\033[1m\033[30m";
	constexpr std::string_view BOLDRED = "\033[1m\033[31m";
	constexpr std::string_view BOLDGREEN = "\033[1m\033[32m";
	constexpr std::string_view BOLDYELLOW = "\033[1m\033[33m";
	constexpr std::string_view BOLDBLUE = "\033[1m\033[34m";
	constexpr std::string_view BOLDMAGENTA = "\033[1m\033[35m";
	constexpr std::string_view BOLDCYAN = "\033[1m\033[36m";
	constexpr std::string_view BOLDWHITE = "\033[1m\033[37m";
}

constexpr std::string_view toColor(ELogLevel level)
{
	switch (level)
	{
		case ELogLevel::Verbose: return LogColors::GREY;
		case ELogLevel::Log: return LogColors::WHITE;
		case ELogLevel::Warning: return LogColors::YELLOW;
		case ELogLevel::Error: return LogColors::RED;
		default: return LogColors::MAGENTA;
	}
}

void logToStream(const std::string_view& channel, ELogLevel level, const std::string_view& log, std::ostream& stream)
{
#if MANI_CONSOLE_APP
	std::println("{}[{}][{}]: {}{}", toColor(level), TimeSystem::getTimeFormatted(), channel, log, LogColors::RESET);
#else
#if MANI_WINDOWS
	std::stringstream ss;
	ss << toColor(level);
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
	std::ofstream fileStream;
	std::atomic<bool> isSuppressed = false;
};

void Logger::log(const LogChannel& channel, ELogLevel level, const std::string_view& log)
{
	if (!m_state->isSuppressed)
	{
		if (level >= channel.level)
		{
			logToStream(channel.name, level, log, std::cout);
		}
	}

	if (m_state->fileStream.is_open())
	{
		logToStream(channel.name, level, log, m_state->fileStream);
	}
}

void Mani::Logger::setFile(const std::filesystem::path& path)
{
	MANI_ASSERT_APP_THREAD;

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
	m_state->isSuppressed = true;
}

void Logger::unsuppress()
{
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
