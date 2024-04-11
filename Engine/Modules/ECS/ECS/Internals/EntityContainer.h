#pragma once

#include <ECS/ECS.h>
#include <ECS/Entity.h>
#include <cstdint>
#include <typeindex>

namespace Mani
{
	/*
	 * Holds a collection of entity and manage their component's memory.
	 */
	class IEntityContainer
	{
	public:
		virtual ~IEntityContainer() {};

		// creates a new Entity and returns its id.
		virtual EntityId create() = 0;

		// destroys an entity with specified id.
		virtual bool destroy(EntityId entityId) = 0;

		// returns a const ref to an entity object
		virtual const Entity* getEntity(EntityId entityId) const = 0;

		// returns the number of entities
		virtual size_t size() const = 0;

		// returns true if an entity with specified id exists
		virtual bool isValid(EntityId entityId) const = 0;

		// allocate a new component with componentId and assigns it to entity
		virtual void* addComponent(EntityId entityId, ComponentId componentId, size_t componentSize) = 0;

		// gets the memory block associated with componentId for entityId
		virtual void* getComponent(EntityId entityId, ComponentId componentId) const = 0;

		virtual ComponentId getComponentId(const std::type_index& typeIndex) = 0;

		// removes the component for entityId
		virtual bool removeComponent(EntityId entityId, ComponentId componentId) = 0;

		// returns true if entityId has componentId
		virtual bool hasComponent(EntityId entityId, ComponentId componentId) const = 0;
	};
}