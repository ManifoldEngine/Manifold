#include "Application.h"
#include <Core/System/SystemContainer.h>
#include <Core/CoreTime.h>

#include <iostream>

using namespace ECSEngine;

Application::Application()
{
	m_pSystemContainer = new SystemContainer();
}

Application::~Application()
{
	delete m_pSystemContainer;
}

SystemContainer& Application::getSystemContainer()
{
	return *m_pSystemContainer;
}

void Application::run()
{
	Time::onApplicationStart();
	m_pSystemContainer->initialize();
	m_world.initialize();
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
	m_pSystemContainer->tick(deltaTime);
	m_world.tick(deltaTime);
}
