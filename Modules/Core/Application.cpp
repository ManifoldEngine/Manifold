#pragma once

#include "Application.h"
#include <Core/System/SystemContainer.h>
#include <Core/CoreTime.h>
#include <Core/Assert.h>
#include <Core/Log/LogSystem.h>

using namespace ECSEngine;

Application* Application::sm_pApplication = nullptr;

Application::Application()
{
	// there should be only one application instance.
	ESCE_ASSERT(sm_pApplication == nullptr);
	sm_pApplication = this;

	m_pSystemContainer = new SystemContainer();
	m_pSystemContainer->createSystem<LogSystem>();
}

Application::~Application()
{
	m_pSystemContainer->destroySystem<LogSystem>();
	delete m_pSystemContainer;
}

Application& ECSEngine::Application::get()
{
	return *sm_pApplication;
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
