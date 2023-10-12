#pragma once

#include "ECS.h"
#include "Entity.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>

// TODO: do not use STL for exposed classes.
// warning C4251: 'ECSEngine::EntityRegistry::ComponentStore': class 'std::vector<unsigned char,std::allocator<unsigned char>>' needs to have dll-interface to be used by clients of class 'ECSEngine::EntityRegistry'
#pragma warning(disable:4251)

namespace ECSEngine {
	/*
	 * Holds a collection of entity and manage their component's memory.
	 */
	class ECS_API EntityRegistry
	{
#define INITIAL_COMPONENT_COUNT 1000
	public:
		template<typename ...TComponents>
		friend class RegistryView;

		EntityId create();
		bool destroy(EntityId entityId);
		
		template<typename TComponent>
		TComponent* addComponent(EntityId entityId);

		template<typename TComponent>
		TComponent* getComponent(EntityId entityId);
		
		template<typename TComponent>
		bool removeComponent(EntityId entityId);

		template<typename TComponent>
		bool hasComponent(EntityId entityId);

		size_t size() const;
		
		bool isValid(EntityId entityId) const;

		// Converts a TComponent type into a numerical identifier.
		template<typename TComponent>
		static ComponentId getComponentId();

	private:
		static ComponentId s_componentCounter;

		struct ComponentPool {

			ComponentPool(size_t inElementsSize) : elementSize(inElementsSize)
			{
				capacity = INITIAL_COMPONENT_COUNT;
				data = std::vector<unsigned char>(capacity * elementSize, 0);
			};

			inline void* Get(size_t index)
			{
				if (index >= capacity)
				{
					// We don't have enough room. double the capacity until we do.
					while (capacity < index)
					{
						capacity *= 2;
					}

					data.resize(capacity * elementSize);
				}

				return &data[0] + index * elementSize;
			}

			std::vector<unsigned char> data;
			size_t capacity;
			size_t elementSize;
		};

		std::vector<ComponentPool*> m_componentPools;
		std::vector<Entity> m_entities;
		std::vector<EntityId> m_entityPool;
		
		bool removeComponent_Internal(EntityId entityId, ComponentId componentId);
		bool hasComponent_Internal(EntityId entityId, ComponentId componentId);
	};

	template<typename TComponent>
	inline TComponent* EntityRegistry::addComponent(EntityId entityId)
	{
		if (!isValid(entityId))
		{
			return nullptr;
		}

		ComponentId componentId = getComponentId<TComponent>();

		if (hasComponent_Internal(entityId, componentId))
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
			m_componentPools[componentId] = new ComponentPool(sizeof(TComponent));
		}

		// this is a placement new
		void* buffer = m_componentPools[componentId]->Get(entityId);
		TComponent* component = new (buffer) TComponent();

		Entity& entity = m_entities[entityId];
		entity.components.set(componentId, true);

		return component;
	}

	template<typename TComponent>
	inline TComponent* EntityRegistry::getComponent(EntityId entityId)
	{
		if (!isValid(entityId))
		{
			return nullptr;
		}


		ComponentId componentId = getComponentId<TComponent>();
		if (componentId >= m_componentPools.size())
		{
			// that component doesn't have a pool yet.
			return nullptr;
		}

		if (!hasComponent_Internal(entityId, componentId))
		{
			return nullptr;
		}

		return static_cast<TComponent*>(m_componentPools[componentId]->Get(entityId));
	}

	template<typename TComponent>
	inline bool EntityRegistry::removeComponent(EntityId entityId)
	{
		ComponentId componentId = getComponentId<TComponent>();
		return removeComponent_Internal(entityId, componentId);
	}

	template<typename TComponent>
	inline bool EntityRegistry::hasComponent(EntityId entityId)
	{
		size_t componentId = getComponentId<TComponent>();
		return hasComponent_Internal(entityId, componentId);
	}

	template<typename TComponent>
	inline ComponentId EntityRegistry::getComponentId()
	{
		static ComponentId s_componentId = s_componentCounter++;
		return s_componentId;
	}
}

#pragma warning(default:4251)