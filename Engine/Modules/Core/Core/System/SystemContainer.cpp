#include "SystemContainer.h"
#include <CoreAssert.h>

using namespace Mani;

void SystemContainer::initialize()
{
	if (m_isInitialized)
	{
		return;
	}

	for (auto& system : m_systems)
	{
		system->initialize(m_registry, *this);
	}

	m_isInitialized = true;
}

void SystemContainer::deinitialize()
{
	if (!m_isInitialized)
	{
		return;
	}

	for (auto it = m_systems.rbegin(); it != m_systems.rend(); it++)
	{
		(*it)->deinitialize(m_registry);
	}

	m_isInitialized = false;
}

void SystemContainer::tick(float deltaTime)
{
	if (!m_isInitialized)
	{
		return;
	}

	for (auto& system : m_systems)
	{
		if (system->shouldTick(m_registry))
		{
			system->tick(deltaTime, m_registry);
		}
	}
}

size_t SystemContainer::size() const
{
	return m_systems.size();
}
