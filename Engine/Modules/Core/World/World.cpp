#include "World.h"
#include "System/SystemContainer.h"

using namespace ECSEngine;

World::World()
{
	m_pSystemContainer = new SystemContainer();
}

World::~World()
{
	deinitialize();
	delete m_pSystemContainer;
}

void World::initialize()
{
	if (m_bIsInitialized)
	{
		return;
	}

	m_pSystemContainer->initialize();
	m_bIsInitialized = true;
}

void World::deinitialize()
{
	if (!m_bIsInitialized)
	{
		return;
	}

	m_pSystemContainer->deinitialize();
	m_bIsInitialized = false;
}

void World::tick(float deltaTime)
{
	m_pSystemContainer->tick(deltaTime);
}

SystemContainer& World::getSystemContainer()
{
	return *m_pSystemContainer;
}
