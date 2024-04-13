#include "Application.h"
#include <Core/System/SystemContainer.h>
#include <Core/World/WorldSystem.h>
#include <Core/CoreTime.h>
#include <Core/ManiAssert.h>
#include <Core/Log/LogSystem.h>

using namespace Mani;

Application* Application::s_application = nullptr;

Application::Application()
{
	// there should be only one application instance.
	MANI_ASSERT(s_application == nullptr, "an Application instance already exists.");
	s_application = this;

	m_systemContainer = new SystemContainer();
	m_systemContainer->initialize();
	m_systemContainer->createSystem<LogSystem>()
		.createSystem<WorldSystem>();
}

Application::~Application()
{
	m_systemContainer->deinitialize();
	m_systemContainer->destroySystem<WorldSystem>()
		.destroySystem<LogSystem>();
	delete m_systemContainer;
	s_application = nullptr;
}

Application& Mani::Application::get()
{
	return *s_application;
}

SystemContainer& Application::getSystemContainer()
{
	return *m_systemContainer;
}

void Application::run()
{
	Time::onApplicationStart();
	m_systemContainer->initialize();
	m_isRunning = true;

	while (m_isRunning)
	{
		Time::onNewFrame();
		tick(Time::getDeltaTime());
	}
	m_systemContainer->deinitialize();
}

void Application::stop()
{
	m_isRunning = false;
}

void Application::tick(float deltaTime)
{
	m_systemContainer->tick(deltaTime);
}
