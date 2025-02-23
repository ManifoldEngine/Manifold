#include "EntityContainer.h"
#include <assert.h>

using namespace Mani;

// EntityContainer::ComponentPool begin

ECS::EntityContainer::ComponentPool::ComponentPool(size_t inElementsSize)
	: elementSize(inElementsSize)
{
	capacity = INITIAL_COMPONENT_COUNT;
	data = std::vector<unsigned char>(capacity * elementSize, 0);
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
	if (m_entities.size() >= UINT64_MAX && m_entityPool.size() == 0)
	{
		return ECS::INVALID_ID;
	}

	if (m_entityPool.size() > 0)
	{
		ECS::EntityId id = m_entityPool.back();
		m_entityPool.pop_back();
		m_entities[id].isAlive = true;
		return id;
	}

	m_entities.push_back(ECS::Entity());
	m_entities.back().id = m_entities.size() - 1;
	m_entities.back().isAlive = true;
	return m_entities.back().id;
}

bool ECS::EntityContainer::destroy(ECS::EntityId entityId)
{
	if (!isValid(entityId))
	{
		return false;
	}

	ECS::Entity& entity = m_entities[entityId];
	entity.isAlive = false;
	entity.resetComponentBits();

	m_entityPool.push_back(entity.id);
	return true;
}

const ECS::Entity* ECS::EntityContainer::getEntity(ECS::EntityId entityId) const
{
	if (!isValid(entityId))
	{
		return nullptr;
	}

	return &m_entities[entityId];
}

size_t ECS::EntityContainer::size() const
{
	return m_entities.size() - m_entityPool.size();
}

size_t ECS::EntityContainer::unadjustedSize() const
{
	return m_entities.size();
}

bool ECS::EntityContainer::isValid(ECS::EntityId entityId) const
{
	if (entityId >= m_entities.size() || entityId == ECS::INVALID_ID)
	{
		return false;
	}

	return m_entities[entityId].isAlive;
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

	return m_componentPools[componentId]->get(entityId);	
}

void* ECS::EntityContainer::getComponent(ECS::EntityId entityId, ComponentId componentId) const
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

	return m_componentPools[componentId]->get(entityId);
}

bool ECS::EntityContainer::removeComponent(ECS::EntityId entityId, ComponentId componentId)
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

bool ECS::EntityContainer::hasComponent(ECS::EntityId entityId, ComponentId componentId) const
{
	if (!isValid(entityId))
	{
		return false;
	}

	return m_entities[entityId].hasComponent(componentId);
}

ECS::ComponentId ECS::EntityContainer::getComponentId(const std::type_index& typeIndex) const
{
	std::unordered_map<std::type_index, ComponentId>* componentIds = const_cast<std::unordered_map<std::type_index, ComponentId>*>(&m_componentIds);
	if (auto it = componentIds->find(typeIndex); it != componentIds->end())
	{
		return it->second;
	}
	else
	{
		assert(componentIds->size() <= MAX_COMPONENTS);
		ComponentId componentId = static_cast<ComponentId>(componentIds->size());
		(*componentIds)[typeIndex] = componentId;
		return componentId;
	}
}
