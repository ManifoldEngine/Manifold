#include "WorldSystem.h"
#include <Core/World/World.h>
#include <vector>

using namespace Mani;

struct WorldSystem::Impl
{
	void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
	{
		for (auto& world : m_worlds)
		{
			world->initialize();
		}
	}

	void onDeinitialize(EntityRegistry& registry)
	{
		for (auto& world : m_worlds)
		{
			world->deinitialize();
		}
	}

	std::shared_ptr<World> createWorld()
	{
		std::shared_ptr<World> world = std::make_shared<World>();
		m_worlds.push_back(world);
		world->initialize();
		return world;
	}

	bool destroyWorld(const std::shared_ptr<World>& world)
	{
		auto it = m_worlds.erase(std::find(m_worlds.begin(), m_worlds.end(), world));
		return it != m_worlds.end();
	}

	void setRelevantWorld(const std::shared_ptr<World>& world)
	{
		m_relevantWorld = world;
	}

	std::shared_ptr<World> getRelevantWorld() const
	{
		return m_relevantWorld;
	}

	void tick(float deltaTime, EntityRegistry& registry)
	{
		for (auto& world : m_worlds)
		{
			world->tick(deltaTime);
		}
	}
private:
	std::vector<std::shared_ptr<World>> m_worlds;
	std::shared_ptr<World> m_relevantWorld = nullptr;
};

WorldSystem::WorldSystem()
{
	m_pImpl = new WorldSystem::Impl();
}

WorldSystem::~WorldSystem()
{
	delete m_pImpl;
}

void WorldSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	if (isInitialized())
	{
		return;
	}
	m_pImpl->onInitialize(registry, systemContainer);
}

void WorldSystem::onDeinitialize(EntityRegistry& registry)
{
	if (!isInitialized())
	{
		return;
	}
	m_pImpl->onDeinitialize(registry);
}

std::shared_ptr<World> WorldSystem::createWorld()
{
	return m_pImpl->createWorld();
}

bool WorldSystem::destroyWorld(const std::shared_ptr<World>& world)
{
	return m_pImpl->destroyWorld(world);
}

void WorldSystem::setRelevantWorld(const std::shared_ptr<World>& world)
{
	m_pImpl->setRelevantWorld(world);
}

std::shared_ptr<World> WorldSystem::getRelevantWorld() const
{
	return m_pImpl->getRelevantWorld();
}

void WorldSystem::tick(float deltaTime, EntityRegistry& registry)
{
	m_pImpl->tick(deltaTime, registry);
}
