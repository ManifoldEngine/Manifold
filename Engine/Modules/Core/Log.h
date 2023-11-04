#pragma once
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Log/LogSystem.h>
#include <format>

namespace ECSEngine 
{
	const std::string_view Log = "Log";
}

#define _ECSE_GET_LOG_SYSTEM ECSEngine::Application::get().getSystemContainer().getSystem<ECSEngine::LogSystem>().lock()

#define ECSE_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE, ...) LogSystem::s_log(CHANNELNAME, ELogLevel::Verbose, std::format(LOGMESSAGE, __VA_ARGS__))
#define ECSE_LOG(CHANNELNAME, LOGMESSAGE, ...) LogSystem::s_log(CHANNELNAME, ELogLevel::Log, std::format(LOGMESSAGE, __VA_ARGS__))
#define ECSE_LOG_WARNING(CHANNELNAME, LOGMESSAGE, ...) LogSystem::s_log(CHANNELNAME, ELogLevel::Warning, std::format(LOGMESSAGE, __VA_ARGS__))
#define ECSE_LOG_ERROR(CHANNELNAME, LOGMESSAGE, ...) LogSystem::s_log(CHANNELNAME, ELogLevel::Error, std::format(LOGMESSAGE, __VA_ARGS__))

#define SET_CHANNEL_LOG_LEVEL(CHANNELNAME, LOGLEVEL) _ECSE_GET_LOG_SYSTEM->setChannelLogLevel(CHANNELNAME, LOGLEVEL)