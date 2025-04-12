#include "Application.h"
#include <Core/World/WorldSystem.h>
#include <Core/CoreTime.h>
#include <Core/ManiAssert.h>
#include <Core/Log/LogSystem.h>

#if MANI_DEBUG
#include <Debug/ProfilingSystem.h>
#endif

constexpr size_t THREAD_COUNT = 10; // todo make this configurable ??

using namespace Mani;

Application* Application::s_application = nullptr;

Application::Application()
	: m_threadPool(THREAD_COUNT)
{
	// there should be only one application instance.
	MANI_ASSERT(s_application == nullptr, "an Application instance already exists.");
	s_application = this;

	m_world.initialize();
	m_world.createSystem<LogSystem>()
		.createSystem<WorldSystem>();

#if MANI_DEBUG
	m_world.createSystem<ProfilingSystem>();
#endif
}

Application::~Application()
{
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
