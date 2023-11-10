#include "World.h"
#include "System/SystemContainer.h"

using namespace ECSEngine;

World::World()
{
	m_systemContainer = new SystemContainer();
}

World::~World()
{
	deinitialize();
	delete m_systemContainer;
}

void World::initialize()
{
	if (m_isInitialized)
	{
		return;
	}

	m_systemContainer->initialize();
	m_isInitialized = true;
}

void World::deinitialize()
{
	if (!m_isInitialized)
	{
		return;
	}

	m_systemContainer->deinitialize();
	m_isInitialized = false;
}

void World::tick(float deltaTime)
{
	m_systemContainer->tick(deltaTime);
}

SystemContainer& World::getSystemContainer()
{
	return *m_systemContainer;
}
