#include "World.h"
#include "System/SystemContainer.h"

using namespace ECSEngine;

World::World()
{
	m_pSystemContainer = new SystemContainer();
}

World::~World()
{
	delete m_pSystemContainer;
}

void World::initialize()
{
	if (m_bIsInitialized)
	{
		return;
	}

	m_pSystemContainer->initialize();
}

void World::tick(float deltaTime)
{
	m_pSystemContainer->tick(deltaTime);
}

SystemContainer& World::getSystemContainer()
{
	return *m_pSystemContainer;
}
