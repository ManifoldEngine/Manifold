#include "EntityRegistry.h"

ECSEngine::EntityId ECSEngine::EntityRegistry::Create()
{
	if (Entities.size() >= UINT64_MAX && EntityPool.size() == 0)
	{
		return UINT64_MAX;
	}

	if (EntityPool.size() > 0)
	{
		EntityId id = EntityPool.back();
		EntityPool.pop_back();
		Entities[id].IsAlive = true;
		return id;
	}

	Entity entity;
	entity.Id = Entities.size();
	entity.IsAlive = true;
	Entities.push_back(entity);
	return entity.Id;
}

bool ECSEngine::EntityRegistry::Destroy(EntityId entityId)
{
	if (!IsValid(entityId))
	{
		return false;
	}

	Entity& entity = Entities[entityId];
	entity.IsAlive = false;
	for (size_t i = 0; i < s_componentCounter; ++i)
	{
		// remove all components
		entity.Components.set(i, false);
	}

	EntityPool.push_back(entity.Id);
	return true;
}

bool ECSEngine::EntityRegistry::IsValid(EntityId entityId) const
{
	if (entityId >= Entities.size() && entityId == UINT64_MAX)
	{
		return false;
	}

	return Entities[entityId].IsAlive;
}

bool ECSEngine::EntityRegistry::RemoveComponent_Internal(EntityId entityId, ComponentId componentId)
{
	if (!IsValid(entityId))
	{
		return false;
	}

	if (!HasComponent_Internal(entityId, componentId))
	{
		// Entity doesn't have a component of that type.
		return false;
	}

	Entity& entity = Entities[entityId];
	entity.Components.set(componentId, false);
	return true;
}

bool ECSEngine::EntityRegistry::HasComponent_Internal(EntityId entityId, ComponentId typeId)
{
	if (!IsValid(entityId))
	{
		return false;
	}

	return Entities[entityId].Components.test(typeId);
}
