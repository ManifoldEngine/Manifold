#pragma once

#include <ECS.h>
#include "Entity.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>

namespace Mani 
{
	namespace ECS
	{
		class EntityContainer
		{

	#define INITIAL_COMPONENT_COUNT 1000

		public:
			virtual ~EntityContainer() = default;
			ECS::EntityId create();
			bool destroy(ECS::EntityId entityId);
			void deferDestroy(ECS::EntityId entityId); // destroys at the end of the tick.
			const Entity* getEntity(ECS::EntityId entityId) const;
			size_t size() const;
			size_t unadjustedSize() const;
			bool isValid(ECS::EntityId entityId) const;

			void* addComponent(ECS::EntityId entityId, ComponentId componentId, size_t componentSize);
			void* getComponent(ECS::EntityId entityId, ComponentId componentId) const;
			ComponentId getComponentId(const std::type_index& typeIndex) const;
			bool removeComponent(ECS::EntityId entityId, ComponentId componentId);
			bool hasComponent(ECS::EntityId entityId, ComponentId componentId) const;

			bool isMarkedForDestroy(ECS::EntityId entityId) const;
			void handleDeferredDestroy();
		private:
			struct ComponentPool 
			{
				ComponentPool(size_t inElementsSize);

				inline void* get(size_t index);
			
				std::vector<unsigned char> data;
				size_t capacity;
				size_t elementSize;
			};

			std::vector<ComponentPool*> m_componentPools;
			std::vector<Entity> m_entities;
			std::vector<ECS::EntityId> m_entityPool;
			std::unordered_set<ECS::EntityId> m_markedForDestroy;
			std::vector<std::pair<std::type_index, ComponentId>> m_componentIds;
		};
	}
}