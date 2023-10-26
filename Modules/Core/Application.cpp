#include "Application.h"
#include <Core/System/SystemContainer.h>
#include <Core/CoreTime.h>

#include <iostream>

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
	Time::onApplicationStart();
	while (m_bIsRunning)
	{
		Time::onNewFrame();
		tick(Time::getDeltaTime());
	}
}

void Application::stop()
{
	m_bIsRunning = false;
}

void Application::tick(float deltaTime)
{
	m_systemContainer->tick(deltaTime);
	m_world.tick(deltaTime);
}
