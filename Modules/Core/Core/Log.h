#pragma once

#include <Core/Logger.h>
#include <string_view>
#include <format>
#include <limits>

namespace Mani 
{
	constexpr LogChannel Log("Log");
}

#if MANI_DEBUG && !MANI_WEBGL
	#define MANI_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE, ...) Mani::Logger::get().log(CHANNELNAME, Mani::ELogLevel::Verbose, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG(CHANNELNAME, LOGMESSAGE, ...) Mani::Logger::get().log(CHANNELNAME, Mani::ELogLevel::Log, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG_WARNING(CHANNELNAME, LOGMESSAGE, ...) Mani::Logger::get().log(CHANNELNAME, Mani::ELogLevel::Warning, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG_ERROR(CHANNELNAME, LOGMESSAGE, ...) Mani::Logger::get().log(CHANNELNAME, Mani::ELogLevel::Error, std::format(LOGMESSAGE, __VA_ARGS__))
#else
	#define MANI_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE, ...)
	#define MANI_LOG(CHANNELNAME, LOGMESSAGE, ...)
	#define MANI_LOG_WARNING(CHANNELNAME, LOGMESSAGE, ...)
	#define MANI_LOG_ERROR(CHANNELNAME, LOGMESSAGE, ...)
#endif