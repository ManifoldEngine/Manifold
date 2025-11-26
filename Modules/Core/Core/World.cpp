#include "World.h"
#include <Core/Debug/Profiling.h>

using namespace Mani;

void World::initialize()
{
	if (m_isInitialized)
	{
		return;
	}

	for (auto& container : m_systems)
	{
		container.system->initialize(m_registry, *this);
	}

	m_isInitialized = true;
}

void World::deinitialize()
{
	if (!m_isInitialized)
	{
		return;
	}

	for (SizeT i = m_systems.count() - 1; i != INDEX_NONE; i--)
	{
		auto& container = m_systems[i];
		container.system->deinitialize(m_registry, *this);
	}

	// it is possible we have deferred entities left.
	m_registry.handleDeferredDestroy();

	m_isInitialized = false;
}

void World::tick()
{
	if (!m_isInitialized)
	{
		return;
	}

	{
		// snapshot the systems that should tick. 
		// New systems can be created during a tick and they might not be in a proper state to tick yet.
		List<SystemContainer> systems = m_systems;
		for (auto& container : systems)
		{
			if (!container.isMarkedForDestruction && container.system->shouldTick(m_registry))
			{
				MANI_TIME_SCOPE(container.system->getName());
				container.system->tick(m_registry);
			}
		}
	}

	m_registry.handleDeferredDestroy();

	// remove uninitialized systems
	m_systems.removeIf([](const auto& container) { return container.isMarkedForDestruction; });
}

size_t World::systemCount() const
{
	return m_systems.count();
}