#pragma once

#include <ECS.h>
#include "Entity.h"
#include <vector>
#include <unordered_map>
#include <typeindex>

namespace Mani 
{
	namespace ECS
	{
		class EntityContainer
		{

	#define INITIAL_COMPONENT_COUNT 1000

		public:
			// IEntityContainer begin
			virtual ~EntityContainer() = default;
			ECS::EntityId create();
			bool destroy(ECS::EntityId entityId);
			const Entity* getEntity(ECS::EntityId entityId) const;
			size_t size() const;
			size_t unadjustedSize() const;
			bool isValid(ECS::EntityId entityId) const;

			void* addComponent(ECS::EntityId entityId, ComponentId componentId, size_t componentSize);
			void* getComponent(ECS::EntityId entityId, ComponentId componentId) const;
			ComponentId getComponentId(const std::type_index& typeIndex) const;
			bool removeComponent(ECS::EntityId entityId, ComponentId componentId);
			bool hasComponent(ECS::EntityId entityId, ComponentId componentId) const;
			// IEntityContainer end

		private:
			struct ComponentPool 
			{
				ComponentPool(size_t inElementsSize);

				inline void* Get(size_t index);
			
				std::vector<unsigned char> data;
				size_t capacity;
				size_t elementSize;
			};

			std::vector<ComponentPool*> m_componentPools;
			std::vector<Entity> m_entities;
			std::vector<ECS::EntityId> m_entityPool;
			std::unordered_map<std::type_index, ComponentId> m_componentIds;
		};
	}
}