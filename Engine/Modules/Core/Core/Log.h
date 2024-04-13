#pragma once

#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Log/LogSystem.h>
#include <format>

namespace Mani 
{
	const std::string_view Log = "Log";
}

#ifndef MANI_WEBGL
	#define MANI_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Verbose, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Log, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG_WARNING(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Warning, std::format(LOGMESSAGE, __VA_ARGS__))
	#define MANI_LOG_ERROR(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Error, std::format(LOGMESSAGE, __VA_ARGS__))
#else
	#define MANI_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Verbose, LOGMESSAGE)
	#define MANI_LOG(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Log, LOGMESSAGE)
	#define MANI_LOG_WARNING(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Warning, LOGMESSAGE)
	#define MANI_LOG_ERROR(CHANNELNAME, LOGMESSAGE, ...) Mani::LogSystem::s_log(CHANNELNAME, Mani::ELogLevel::Error, LOGMESSAGE)
#endif

#define MANI_SET_CHANNEL_LOG_LEVEL(CHANNELNAME, LOGLEVEL) Mani::LogSystem::s_setChannelLogLevel(CHANNELNAME, LOGLEVEL)