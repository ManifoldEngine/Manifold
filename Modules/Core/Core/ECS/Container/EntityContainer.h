#pragma once

#include <Core/ECS/Entity.h>
#include <Core/Containers/List.h>
#include <typeindex>
#include <mutex>

namespace Mani 
{
	namespace ECS
	{
		class EntityContainer
		{
		public:
			virtual ~EntityContainer() = default;
			ECS::EntityId create();
			bool destroy(ECS::EntityId entityId);
			void deferDestroy(ECS::EntityId entityId); // destroys at the end of the tick.
			const Entity* getEntity(ECS::EntityId entityId) const;
			const Entity* getEntityAt(ECS::Index index) const;
			ECS::Index count() const;
			ECS::Index unadjustedCount() const;
			bool isValid(ECS::EntityId entityId) const;
			bool isValidIndex(ECS::Index index) const;

			void* addComponent(ECS::EntityId entityId, ComponentId componentId, size_t componentSize);
			void* getComponent(ECS::EntityId entityId, ComponentId componentId) const;
			void* removeComponent(ECS::EntityId entityId, ComponentId componentId);
			bool hasComponent(ECS::EntityId entityId, ComponentId componentId) const;

			bool isMarkedForDestroy(ECS::EntityId entityId) const;
			void handleDeferredDestroy();
		private:
			struct ComponentPool 
			{
				ComponentPool(size_t inElementsSize);

				inline void* get(size_t index);
			
				Mani::List<unsigned char> data;
				size_t capacity;
				size_t elementSize;
			};

			Mani::List<ComponentPool*> m_componentPools;
			Mani::List<Entity> m_entities;
			Mani::List<ECS::Index> m_entityPool;
			Mani::List<ECS::EntityId> m_markedForDestroy;
			mutable std::mutex m_markedForDestroyMutex;
		};
	}
}