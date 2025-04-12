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

	m_systemContainer.initialize();
	m_systemContainer.createSystem<LogSystem>()
					 .createSystem<WorldSystem>();

#if MANI_DEBUG
	m_systemContainer.createSystem<ProfilingSystem>();
#endif
}

Application::~Application()
{
	m_systemContainer.deinitialize();

#if MANI_DEBUG
	m_systemContainer.destroySystem<ProfilingSystem>();
#endif

	m_systemContainer.destroySystem<WorldSystem>()
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
	m_systemContainer.initialize();
	m_isRunning = true;

	while (m_isRunning)
	{
		Time::onNewFrame();
		tick(Time::getDeltaTime());
	}
	m_systemContainer.deinitialize();
}

void Application::stop()
{
	m_isRunning = false;
}

void Application::tick(float deltaTime)
{
	m_systemContainer.tick(deltaTime);
}
