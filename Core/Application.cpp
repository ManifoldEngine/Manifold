#include "Application.h"
#include <iostream>

using namespace ECSEngine;

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

void ECSEngine::Application::registerTickable(ITickable* pTickable)
{
}

void ECSEngine::Application::unRegisterTickable(ITickable* pTickable)
{
}
