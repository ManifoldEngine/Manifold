#include "World.h"

using namespace ECSEngine;

void World::tick(float deltaTime)
{
	for (auto& system : m_systems)
	{
		if (system->shouldTick(m_registry))
		{
			system->tick(deltaTime, m_registry);
		}
	}
}
