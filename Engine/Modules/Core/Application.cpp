#pragma once

#include "Application.h"
#include <Core/System/SystemContainer.h>
#include <Core/World/WorldSystem.h>
#include <Core/CoreTime.h>
#include <Core/Assert.h>
#include <Core/Log/LogSystem.h>

using namespace ECSEngine;

Application* Application::sm_pApplication = nullptr;

Application::Application()
{
	// there should be only one application instance.
	ECSE_ASSERT(sm_pApplication == nullptr, "an Application instance already exists.");
	sm_pApplication = this;

	m_pSystemContainer = new SystemContainer();
	m_pSystemContainer->initialize();
	m_pSystemContainer->createSystem<LogSystem>();
	m_pSystemContainer->createSystem<WorldSystem>();
}

Application::~Application()
{
	m_pSystemContainer->deinitialize();
	m_pSystemContainer->destroySystem<LogSystem>();
	m_pSystemContainer->destroySystem<WorldSystem>();
	delete m_pSystemContainer;
	sm_pApplication = nullptr;
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
	m_bIsRunning = true;
	while (m_bIsRunning)
	{
		Time::onNewFrame();
		tick(Time::getDeltaTime());
	}
	m_pSystemContainer->deinitialize();
}

void Application::stop()
{
	m_bIsRunning = false;
}

void Application::tick(float deltaTime)
{
	m_pSystemContainer->tick(deltaTime);
}
