#include "Application.h"
#include <Core/System/SystemContainer.h>

using namespace ECSEngine;

Application::Application()
{
	m_systemContainer = new SystemContainer();
}

Application::~Application()
{
	delete m_systemContainer;
}

SystemContainer& Application::getSystemContainer()
{
	return *m_systemContainer;
}

void Application::run()
{
	while (m_bIsRunning)
	{
		tick(.16f);
	}
}

void Application::stop()
{
	m_bIsRunning = false;
}

void Application::tick(float deltaTime)
{
	m_world.tick(.16f);
}
