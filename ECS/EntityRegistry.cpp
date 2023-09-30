#include "EntityRegistry.h"

uint32_t ECSEngine::EntityRegistry::Create()
{
	Entity entity;

	size_t size = Entities.size();
	if (size >= UINT64_MAX)
	{
		return INVALID_ID;
	}

	entity.Id = size;
	Entities.push_back(entity);
	return entity.Id;
}

void ECSEngine::EntityRegistry::Destroy(EntityId entityId)
{
	if (!IsValid(entityId))
	{
		return;
	}

	const Entity& entity = Entities[entityId];
	

	Entities.erase(Entities.begin() + entityId);
}

bool ECSEngine::EntityRegistry::IsValid(EntityId entityId) const
{
	// TODO: this doesn't account for destroyed entities.
	return entityId < Entities.size() && entityId != INVALID_ID;
}

bool ECSEngine::EntityRegistry::AddComponent_Internal(EntityId entityId, ComponentId typeId)
{
	if (!IsValid(entityId))
	{
		return;
	}

	if (HasComponent_Internal(entityId, typeId))
	{
		// Entity already has a component of that type.
		return false;
	}

	Entity& entity = Entities[entityId];
	entity.Components.set(typeId, true);
	
	// todo: allocate component data

	return true;
}

bool ECSEngine::EntityRegistry::RemoveComponent_Internal(EntityId entityId, ComponentId componentId)
{
	if (!IsValid(entityId))
	{
		return;
	}

	if (!HasComponent_Internal(entityId, componentId))
	{
		// Entity doesn't have a component of that type.
		return false;
	}

	Entity& entity = Entities[entityId];
	entity.Components.set(componentId, false);
}

bool ECSEngine::EntityRegistry::HasComponent_Internal(EntityId entityId, ComponentId typeId)
{
	if (!IsValid(entityId))
	{
		return;
	}

	return Entities[entityId].Components.test(typeId);
}

//bool ECSEngine::EntityRegistry::Query_Internal(QueryResult& OutResult, const std::vector<ComponentId>& typeIds)
//{
//	return false;
//}
