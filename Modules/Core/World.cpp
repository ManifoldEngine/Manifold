#include "World.h"
#include "System/SystemContainer.h"

using namespace ECSEngine;

World::World()
{
	m_systemContainer = new SystemContainer();
}

World::~World()
{
	delete m_systemContainer;
}

void World::tick(float deltaTime)
{
	m_systemContainer->tick(deltaTime);
}

SystemContainer& World::getSystemContainer()
{
	return *m_systemContainer;
}
