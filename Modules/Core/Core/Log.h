#pragma once

#include <Core/Log/LogSystem.h>
#include <string_view>
#include <format>

namespace Mani 
{
	const std::string_view Log = "Log";
}

#if MANI_DEBUG && !MANI_WEBGL
	#define MANI_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Verbose, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Log, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG_WARNING(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Warning, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG_ERROR(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Error, std::format(LOGMESSAGE, __VA_ARGS__))

	#define MANI_SET_CHANNEL_LOG_LEVEL(CHANNELNAME, LOGLEVEL) Mani::LogSystem::s_setChannelLogLevel(CHANNELNAME, LOGLEVEL)
#else
	#define MANI_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE, ...)
	#define MANI_LOG(CHANNELNAME, LOGMESSAGE, ...)
	#define MANI_LOG_WARNING(CHANNELNAME, LOGMESSAGE, ...)
	#define MANI_LOG_ERROR(CHANNELNAME, LOGMESSAGE, ...)

	#define MANI_SET_CHANNEL_LOG_LEVEL(CHANNELNAME, LOGLEVEL)
#endif