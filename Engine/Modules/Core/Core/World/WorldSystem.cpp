#include "WorldSystem.h"
#include <Core/World/World.h>
#include <vector>

using namespace Mani;

void WorldSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	if (isInitialized())
	{
		return;
	}

	for (auto& world : m_worlds)
	{
		world->initialize();
	}
}

void WorldSystem::onDeinitialize(EntityRegistry& registry)
{
	if (!isInitialized())
	{
		return;
	}

	for (auto& world : m_worlds)
	{
		world->deinitialize();
	}
}

std::shared_ptr<World> WorldSystem::createWorld()
{
	std::shared_ptr<World> world = std::make_shared<World>();
	m_worlds.push_back(world);
	world->initialize();
	return world;
}

bool WorldSystem::destroyWorld(const std::shared_ptr<World>& world)
{
	auto it = m_worlds.erase(std::find(m_worlds.begin(), m_worlds.end(), world));
	return it != m_worlds.end();
}

void WorldSystem::setRelevantWorld(const std::shared_ptr<World>& world)
{
	m_relevantWorld = world;
}

std::shared_ptr<World> WorldSystem::getRelevantWorld() const
{
	return m_relevantWorld;
}

void WorldSystem::tick(float deltaTime, EntityRegistry& registry)
{
	for (auto& world : m_worlds)
	{
		world->tick(deltaTime);
	}
}
