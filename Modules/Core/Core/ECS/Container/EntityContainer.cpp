#include "EntityContainer.h"
#include <assert.h>

using namespace Mani;

constexpr size_t INITIAL_COMPONENT_COUNT = 10000;

// EntityContainer::ComponentPool begin

ECS::EntityContainer::ComponentPool::ComponentPool(size_t inElementsSize)
	: elementSize(inElementsSize)
{
	capacity = INITIAL_COMPONENT_COUNT;
	data = Mani::List<unsigned char>(capacity * elementSize, 0);
}

void* ECS::EntityContainer::ComponentPool::get(size_t index)
{
	if (index >= capacity)
	{
		// We don't have enough room. double the capacity until we do.
		while (index >= capacity)
		{
			capacity *= 2;
		}

		data.resize(capacity * elementSize);
	}

	return &data[0] + index * elementSize;
}

// EntityContainer::ComponentPool end

ECS::EntityId ECS::EntityContainer::create()
{
	if (m_entities.count() >= UINT64_MAX && m_entityPool.count() == 0)
	{
		return ECS::INVALID_ID;
	}

	ECS::Entity* entity = nullptr;
	if (m_entityPool.count() > 0)
	{
		// get entity from entity pool
		ECS::EntityId id = m_entityPool.pop();
		entity = &m_entities[id];
		const Version newVersion = entity->getVersion() + 1;
		entity->setVersion(newVersion);

#if MANI_DEBUG
		if (entity->getVersion() >= ECS::MAX_VERSION)
		{
			MANI_LOG_WARNING(LogCore, "Entity version loopback");
		}
#endif
	}
	else
	{
		// create new entity
		MANI_ASSERT(m_entities.count() < ECS::MAX_INDEX, "We're at entity capacity");
		m_entities.add(ECS::Entity());
		entity = &m_entities.last();
		entity->setIndex(static_cast<ECS::Index>(m_entities.count() - 1));
		entity->setVersion(0);
	}

	entity->isAlive = true;
	return entity->getId();
}

bool ECS::EntityContainer::destroy(ECS::EntityId entityId)
{
	if (!isValid(entityId))
	{
		return false;
	}

	const ECS::Index index = ECS::toIndex(entityId);
	ECS::Entity& entity = m_entities[index];
	entity.isAlive = false;
	entity.resetComponentBits();

	m_entityPool.add(entity.getIndex());
	return true;
}

void ECS::EntityContainer::deferDestroy(ECS::EntityId entityId)
{
	std::lock_guard<std::mutex> lock(m_markedForDestroyMutex);
	m_markedForDestroy.addUnique(entityId);
}

const ECS::Entity* ECS::EntityContainer::getEntity(ECS::EntityId entityId) const
{
	if (!isValid(entityId))
	{
		return nullptr;
	}

	const ECS::Index index = ECS::toIndex(entityId);
	return &m_entities[index];
}

const ECS::Entity* ECS::EntityContainer::getEntityAt(ECS::Index index) const
{
	if (!isValidIndex(index))
	{
		return nullptr;
	}
	return &m_entities[index];
}

ECS::Index ECS::EntityContainer::count() const
{
	return static_cast<ECS::Index>(m_entities.count() - m_entityPool.count());
}

ECS::Index ECS::EntityContainer::unadjustedCount() const
{
	return static_cast<ECS::Index>(m_entities.count());
}

bool ECS::EntityContainer::isValid(ECS::EntityId entityId) const
{
	if (entityId == ECS::INVALID_ID)
	{
		return false;
	}

	const ECS::Index index = ECS::toIndex(entityId);
	if (!isValidIndex(index))
	{
		return false;
	}

	const ECS::Version version = ECS::toVersion(entityId);
	return m_entities[index].getVersion() == version;
}

bool Mani::ECS::EntityContainer::isValidIndex(ECS::Index index) const
{
	return index < m_entities.count() && m_entities[index].isAlive;
}

void* ECS::EntityContainer::addComponent(ECS::EntityId entityId, ComponentId componentId, size_t componentSize)
{
	if (!isValid(entityId))
	{
		return nullptr;
	}

	if (hasComponent(entityId, componentId))
	{
		// Entity already has a component of that type.
		return nullptr;
	}

	if (componentId >= m_componentPools.count())
	{
		m_componentPools.resize(componentId + 1);
	}
	if (m_componentPools[componentId] == nullptr)
	{
		m_componentPools[componentId] = new ComponentPool(componentSize);
	}

	const ECS::Index index = ECS::toIndex(entityId);
	Entity& entity = m_entities[index];
	entity.setComponentBit(componentId);

	return m_componentPools[componentId]->get(index);
}

void* ECS::EntityContainer::getComponent(ECS::EntityId entityId, ComponentId componentId) const
{
	if (!isValid(entityId))
	{
		return nullptr;
	}

	if (componentId >= m_componentPools.count())
	{
		// that component doesn't have a pool yet.
		return nullptr;
	}

	if (!hasComponent(entityId, componentId))
	{
		return nullptr;
	}

	const ECS::Index index = ECS::toIndex(entityId);
	return m_componentPools[componentId]->get(index);
}

void* ECS::EntityContainer::removeComponent(ECS::EntityId entityId, ComponentId componentId)
{
	if (!isValid(entityId))
	{
		return nullptr;
	}

	if (!hasComponent(entityId, componentId))
	{
		// Entity doesn't have a component of that type.
		return nullptr;
	}

	const ECS::Index index = ECS::toIndex(entityId);
	void* data = m_componentPools[componentId]->get(index);
	Entity& entity = m_entities[index];
	entity.resetComponentBit(componentId);
	return data;
}

bool ECS::EntityContainer::hasComponent(ECS::EntityId entityId, ComponentId componentId) const
{
	if (!isValid(entityId))
	{
		return false;
	}

	const ECS::Index index = ECS::toIndex(entityId);
	return m_entities[index].hasComponent(componentId);
}

bool Mani::ECS::EntityContainer::isMarkedForDestroy(ECS::EntityId entityId) const
{
	std::lock_guard<std::mutex> lock(m_markedForDestroyMutex);
	return m_markedForDestroy.contains(entityId);
}

void Mani::ECS::EntityContainer::handleDeferredDestroy()
{
	std::lock_guard<std::mutex> lock(m_markedForDestroyMutex);
	for (const auto entityId : m_markedForDestroy)
	{
		destroy(entityId);
	}
	m_markedForDestroy.clear();
}

