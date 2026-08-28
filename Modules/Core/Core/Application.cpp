#include "Application.h"
#include <Core/CoreConfig.h>
#include <Core/ManiAssert.h>

#include <Core/Log.h>

#include <Core/FileSystem.h>

#include <Core/ManiTime.h>
#include <Core/TimeSystem.h>

#include <ManiZ/Json.h>
#include <thread>
#include <chrono>

using namespace Mani;

using ManiClock = std::chrono::steady_clock;
using ns = std::chrono::nanoseconds;

CoreConfig loadConfig()
{
	CoreConfig config;
	const Path path = FileSystem::getConfigPath().append(Mani::CONFIG_FILENAME);
	std::string content;
	if (FileSystem::readFile(path, content))
	{
		config = ManiZ::from::json<CoreConfig>(content);
		MANI_LOG(Log, "Loaded CoreConfig from {}", path.string());
	}
	else
	{
		MANI_LOG(Log, "Could not find [{}], using default config instead", path.string());
	}
	return config;
}

Application* Application::s_application = nullptr;

Application::Application()
{
	// there should be only one application instance.
	MANI_ASSERT(s_application == nullptr, "an Application instance already exists.");
	s_application = this;

	m_threadId = Mani::thisThreadId();

	m_config = loadConfig();

	m_threadPool.start(m_config.threadPoolSize);

	m_world.initialize();
}

Application::~Application()
{
	m_threadPool.stop();

	m_world.deinitialize();

	s_application = nullptr;
}

Application& Mani::Application::get()
{
	return *s_application;
}

bool Mani::Application::exists()
{
	return s_application != nullptr;
}

void Application::run()
{
	m_world.initialize();
	m_isRunning = true;

	while (m_isRunning)
	{
		tick();
	}
	m_world.deinitialize();
}

void Application::stop()
{
	m_isRunning = false;
}

void Application::tick()
{
	m_world.tick();
	m_deferred.resolve();

#if MANI_PROFILING
	m_profiler.collectRecords();
#endif
}
