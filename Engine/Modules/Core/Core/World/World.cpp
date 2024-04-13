#include "World.h"
#include <Core/System/SystemContainer.h>
#include <Core/TransformSystem.h>

using namespace Mani;

World::World()
{
	m_systemContainer = new SystemContainer();
	m_systemContainer->createSystem<TransformSystem>();
}

World::~World()
{
	deinitialize();
	m_systemContainer->destroySystem<TransformSystem>();
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
