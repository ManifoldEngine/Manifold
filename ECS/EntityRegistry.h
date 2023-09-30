#pragma once

#include "ECS.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include "Entity.h"

// TODO: do not use STL for exposed classes.
// warning C4251: 'ECSEngine::EntityRegistry::ComponentStore': class 'std::vector<unsigned char,std::allocator<unsigned char>>' needs to have dll-interface to be used by clients of class 'ECSEngine::EntityRegistry'
#pragma warning(disable:4251)

namespace ECSEngine {

	class ECS_API EntityRegistry
	{
#define INITIAL_COMPONENT_COUNT 1000
	public:
		EntityId Create();
		bool Destroy(EntityId entityId);
		
		template<typename TComponent>
		TComponent* AddComponent(EntityId entityId);

		template<typename TComponent>
		TComponent* GetComponent(EntityId entityId);
		
		template<typename TComponent>
		bool RemoveComponent(EntityId entityId);

		template<typename TComponent>
		bool HasComponent(EntityId entityId);
		
		bool IsValid(EntityId entityId) const;

	private:
		struct ComponentPool {

			ComponentPool(size_t inElementsSize) : ElementSize(inElementsSize)
			{
				Capacity = INITIAL_COMPONENT_COUNT;
				Data = std::vector<unsigned char>(Capacity * ElementSize, 0);
			};

			inline void* Get(size_t index)
			{
				if (index >= Capacity)
				{
					// We don't have enough room. double the capacity until we do.
					while (Capacity < index)
					{
						Capacity *= 2;
					}

					Data.resize(Capacity * ElementSize);
				}

				return &Data[0] + index * ElementSize;
			}

			std::vector<unsigned char> Data;
			size_t Capacity;
			size_t ElementSize;
		};

		std::vector<ComponentPool*> ComponentPools;
		std::vector<Entity> Entities;
		std::vector<EntityId> EntityPool;
		
		bool RemoveComponent_Internal(EntityId entityId, ComponentId componentId);
		bool HasComponent_Internal(EntityId entityId, ComponentId componentId);

		// Converts a TComponent type into a numerical identifier.
		template<typename TComponent>
		ComponentId GetComponentId() const;
	};

	template<typename TComponent>
	inline TComponent* EntityRegistry::AddComponent(EntityId entityId)
	{
		if (!IsValid(entityId))
		{
			return nullptr;
		}

		ComponentId componentId = GetComponentId<TComponent>();

		if (HasComponent_Internal(entityId, componentId))
		{
			// Entity already has a component of that type.
			return nullptr;
		}

		if (componentId >= ComponentPools.size())
		{
			ComponentPools.resize(componentId + 1, nullptr);
		}
		if (ComponentPools[componentId] == nullptr)
		{
			ComponentPools[componentId] = new ComponentPool(sizeof(TComponent));
		}

		// this is a placement new
		void* buffer = ComponentPools[componentId]->Get(entityId);
		TComponent* component = new (buffer) TComponent();

		Entity& entity = Entities[entityId];
		entity.Components.set(componentId, true);

		return component;
	}

	template<typename TComponent>
	inline TComponent* EntityRegistry::GetComponent(EntityId entityId)
	{
		if (!IsValid(entityId))
		{
			return nullptr;
		}

		ComponentId componentId = GetComponentId<TComponent>();
		if (componentId >= ComponentPools.size())
		{
			// that component doesn't have a pool yet.
			return nullptr;
		}

		return static_cast<TComponent*>(ComponentPools[componentId]->Get(entityId));
	}

	template<typename TComponent>
	inline bool EntityRegistry::RemoveComponent(EntityId entityId)
	{
		ComponentId componentId = GetComponentId<TComponent>();
		return RemoveComponent_Internal(entityId, componentId);
	}

	template<typename TComponent>
	inline bool EntityRegistry::HasComponent(EntityId entityId)
	{
		size_t componentId = GetComponentId<TComponent>();
		return HasComponent_Internal(entityId, componentId);
	}

	// TODO: does this world on all platforms ?
	extern ComponentId s_componentCounter = 0;
	template<typename TComponent>
	inline ComponentId EntityRegistry::GetComponentId() const
	{
		static ComponentId s_componentId = s_componentCounter++;
		return s_componentId;
	}
}

#pragma warning(default:4251)