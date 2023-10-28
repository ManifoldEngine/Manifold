#pragma once
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Log/LogSystem.h>

namespace ECSEngine 
{
	const std::string_view Log = "Log";
}

#define ECSE_LOG_VERBOSE(CHANNELNAME, LOGMESSAGE) ECSEngine::Application::get().getSystemContainer().getSystem<ECSEngine::LogSystem>()->log(CHANNELNAME, ELogLevel::Verbose, LOGMESSAGE)
#define ECSE_LOG(CHANNELNAME, LOGMESSAGE) ECSEngine::Application::get().getSystemContainer().getSystem<ECSEngine::LogSystem>()->log(CHANNELNAME, ELogLevel::Log, LOGMESSAGE)
#define ECSE_LOG_WARNING(CHANNELNAME, LOGMESSAGE) ECSEngine::Application::get().getSystemContainer().getSystem<ECSEngine::LogSystem>()->log(CHANNELNAME, ELogLevel::Warning, LOGMESSAGE)
#define ECSE_LOG_ERROR(CHANNELNAME, LOGMESSAGE) ECSEngine::Application::get().getSystemContainer().getSystem<ECSEngine::LogSystem>()->log(CHANNELNAME, ELogLevel::Error, LOGMESSAGE)

#define SET_CHANNEL_LOG_LEVEL(CHANNELNAME, LOGLEVEL) ECSEngine::Application::get().getSystemContainer().getSystem<ECSEngine::LogSystem>()->setChannelLogLevel(CHANNELNAME, LOGLEVEL)