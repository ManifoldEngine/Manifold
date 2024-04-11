#include "EntityContainer_Implementation.h"
#include <assert.h>

using namespace Mani;

// EntityContainer_Implementation::ComponentPool begin

EntityContainer_Implementation::ComponentPool::ComponentPool(size_t inElementsSize)
	: elementSize(inElementsSize)
{
	capacity = INITIAL_COMPONENT_COUNT;
	data = std::vector<unsigned char>(capacity * elementSize, 0);
}

void* EntityContainer_Implementation::ComponentPool::Get(size_t index)
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

// EntityContainer_Implementation::ComponentPool end

EntityId EntityContainer_Implementation::create()
{
	if (m_entities.size() >= UINT64_MAX && m_entityPool.size() == 0)
	{
		return INVALID_ID;
	}

	if (m_entityPool.size() > 0)
	{
		EntityId id = m_entityPool.back();
		m_entityPool.pop_back();
		m_entities[id].isAlive = true;
		return id;
	}

	m_entities.push_back(Entity());
	m_entities.back().id = m_entities.size() - 1;
	m_entities.back().isAlive = true;
	return m_entities.back().id;
}

bool EntityContainer_Implementation::destroy(EntityId entityId)
{
	if (!isValid(entityId))
	{
		return false;
	}

	Entity& entity = m_entities[entityId];
	entity.isAlive = false;
	entity.resetComponentBits();

	m_entityPool.push_back(entity.id);
	return true;
}

const Entity* EntityContainer_Implementation::getEntity(EntityId entityId) const
{
	if (!isValid(entityId))
	{
		return nullptr;
	}

	return &m_entities[entityId];
}

size_t EntityContainer_Implementation::size() const
{
	return m_entities.size() - m_entityPool.size();
}

bool EntityContainer_Implementation::isValid(EntityId entityId) const
{
	if (entityId >= m_entities.size() && entityId == INVALID_ID)
	{
		return false;
	}

	return m_entities[entityId].isAlive;
}

void* EntityContainer_Implementation::addComponent(EntityId entityId, ComponentId componentId, size_t componentSize)
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

	if (componentId >= m_componentPools.size())
	{
		m_componentPools.resize(componentId + 1, nullptr);
	}
	if (m_componentPools[componentId] == nullptr)
	{
		m_componentPools[componentId] = new ComponentPool(componentSize);
	}

	Entity& entity = m_entities[entityId];
	entity.setComponentBit(componentId);

	return m_componentPools[componentId]->Get(entityId);	
}

void* EntityContainer_Implementation::getComponent(EntityId entityId, ComponentId componentId) const
{
	if (!isValid(entityId))
	{
		return nullptr;
	}

	if (componentId >= m_componentPools.size())
	{
		// that component doesn't have a pool yet.
		return nullptr;
	}

	if (!hasComponent(entityId, componentId))
	{
		return nullptr;
	}

	return m_componentPools[componentId]->Get(entityId);
}

bool EntityContainer_Implementation::removeComponent(EntityId entityId, ComponentId componentId)
{
	if (!isValid(entityId))
	{
		return false;
	}

	if (!hasComponent(entityId, componentId))
	{
		// Entity doesn't have a component of that type.
		return false;
	}

	Entity& entity = m_entities[entityId];
	entity.resetComponentBit(componentId);
	return true;
}

bool EntityContainer_Implementation::hasComponent(EntityId entityId, ComponentId componentId) const
{
	if (!isValid(entityId))
	{
		return false;
	}

	return m_entities[entityId].hasComponent(componentId);
}

ComponentId EntityContainer_Implementation::getComponentId(const std::type_index& typeIndex)
{
	if (auto it = m_componentIds.find(typeIndex); it != m_componentIds.end())
	{
		return it->second;
	}
	else
	{
		assert(m_componentIds.size() <= MAX_COMPONENTS);
		ComponentId componentId = static_cast<ComponentId>(m_componentIds.size());
		m_componentIds[typeIndex] = componentId;
		return componentId;
	}
}
