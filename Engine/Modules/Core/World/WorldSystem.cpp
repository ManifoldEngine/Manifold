#include "WorldSystem.h"
#include <Core/World/World.h>
#include <vector>

using namespace ECSEngine;

struct WorldSystem::Impl
{
	void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
	{
		for (auto& pWorld : m_pWorlds)
		{
			pWorld->initialize();
		}
	}

	void onDeinitialize(EntityRegistry& registry)
	{
		for (auto& pWorld : m_pWorlds)
		{
			pWorld->deinitialize();
		}
	}

	std::shared_ptr<World> createWorld()
	{
		std::shared_ptr<World> pWorld = std::make_shared<World>();
		m_pWorlds.push_back(pWorld);
		pWorld->initialize();
		return pWorld;
	}

	bool destroyWorld(const std::shared_ptr<World>& pWorld)
	{
		auto it = m_pWorlds.erase(std::find(m_pWorlds.begin(), m_pWorlds.end(), pWorld));
		return it != m_pWorlds.end();
	}

	void setRelevantWorld(const std::shared_ptr<World>& pWorld)
	{
		m_pRelevantWorld = pWorld;
	}

	std::shared_ptr<World> getRelevantWorld() const
	{
		return m_pRelevantWorld;
	}

	void tick(float deltaTime, EntityRegistry& registry)
	{
		for (auto& pWorld : m_pWorlds)
		{
			pWorld->tick(deltaTime);
		}
	}
private:
	std::vector<std::shared_ptr<World>> m_pWorlds;
	std::shared_ptr<World> m_pRelevantWorld = nullptr;
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

bool WorldSystem::destroyWorld(const std::shared_ptr<World>& pWorld)
{
	return m_pImpl->destroyWorld(pWorld);
}

void WorldSystem::setRelevantWorld(const std::shared_ptr<World>& pWorld)
{
	m_pImpl->setRelevantWorld(pWorld);
}

std::shared_ptr<World> WorldSystem::getRelevantWorld() const
{
	return m_pImpl->getRelevantWorld();
}

void WorldSystem::tick(float deltaTime, EntityRegistry& registry)
{
	m_pImpl->tick(deltaTime, registry);
}
