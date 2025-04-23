#include "Application.h"
#include <Core/CoreTime.h>
#include <Core/CoreConfig.h>
#include <Core/ManiAssert.h>

#include <Core/World/WorldSystem.h>
#include <Core/Log.h>
#include <Core/Log/LogSystem.h>

#include <Core/FileSystem.h>
#include <ManiZ/Json.h>

#if MANI_DEBUG
#include <Debug/ProfilingSystem.h>
#endif

using namespace Mani;

CoreConfig loadConfig()
{
	CoreConfig config;
	const std::filesystem::path path = FileSystem::getConfigPath().append(Mani::CONFIG_FILENAME);
	std::string content;
	if (FileSystem::readFile(path, content))
	{
		config = ManiZ::from::json<CoreConfig>(content);
		MANI_LOG(Log, "Loaded CoreConfig from {}", path.string());
	}
	else
	{
		MANI_LOG_ERROR(Log, "Could not find [{}], using default config instead", path.string());
	}
	return config;
}

Application* Application::s_application = nullptr;

Application::Application()
{
	// there should be only one application instance.
	MANI_ASSERT(s_application == nullptr, "an Application instance already exists.");
	s_application = this;

	m_config = loadConfig();

	m_threadPool.start(m_config.threadPoolSize);

	m_world.initialize();
	m_world.createSystem<LogSystem>()
		.createSystem<WorldSystem>();

#if MANI_DEBUG
	m_world.createSystem<ProfilingSystem>();
#endif
}

Application::~Application()
{
	m_threadPool.stop();

	m_world.deinitialize();

#if MANI_DEBUG
	m_world.destroySystem<ProfilingSystem>();
#endif

	m_world.destroySystem<WorldSystem>()
		.destroySystem<LogSystem>();

	s_application = nullptr;
}

Application& Mani::Application::get()
{
	return *s_application;
}

void Application::run()
{
	Time::onApplicationStart();
	m_world.initialize();
	m_isRunning = true;

	while (m_isRunning)
	{
		Time::onNewFrame();
		tick(Time::getDeltaTime());
	}
	m_world.deinitialize();
}

void Application::stop()
{
	m_isRunning = false;
}

void Application::tick(float deltaTime)
{
	m_world.tick(deltaTime);
}
