#pragma once

#include "Container/EntityContainer.h"
#include "Entity.h"
#include <iostream>
#include <tuple>

namespace Mani
{
	namespace ECS
	{
		/*
		 * Holds a collection of entity and manage their component's memory.
		 */
		class Registry
		{
#define INITIAL_COMPONENT_COUNT 1000
		public:
			template<typename ...TComponents>
			friend class View;

			Registry();
			~Registry();

			Registry(const Registry&) = delete;
			Registry(Registry&) = delete;

			// creates an entity id
			ECS::EntityId create();

			// destroys an entity and its components
			bool destroy(ECS::EntityId entityId);
			
			// destroys at the end of the tick.
			void deferDestroy(ECS::EntityId entityId); 

			// returs an entity object
			const Entity* getEntity(ECS::EntityId entityId) const;

			// adds a TComponent to an entity
			// returns the added component
			template<typename TComponent, typename... TArgs>
			TComponent* add(ECS::EntityId entityId, TArgs&&... args);

			// adds all TComponents to an entity
			// returns the added components
			template<typename ...TComponent>
			auto addMany(ECS::EntityId entityId);

			// removes a TComponent to an entity
			// returns true if a component was removed
			template<typename TComponent>
			bool remove(ECS::EntityId entityId);

			// removes a TComponent to an entity
			// returns true if a component was removed
			template<typename ...TComponents>
			auto removeMany(ECS::EntityId entityId);

			// returns an entity's TComponent
			template<typename TComponent>
			TComponent* get(ECS::EntityId entityId);

			// returns an entity's const TComponent
			template<typename TComponent>
			const TComponent* get(ECS::EntityId entityId) const;

			// returns an entity's TComponent in a tuple
			template<typename ...TComponents>
			auto getMany(ECS::EntityId entityId);

			// returns an entity's const TComponent in a tuple
			template<typename ...TComponents>
			auto getMany(ECS::EntityId entityId) const;

			// returns true if an entity has a component (fast)
			template<typename TComponent>
			bool has(ECS::EntityId entityId) const;

			// adds a singleton TComponent
			// returns the added component
			template<typename TComponent>
			TComponent* addSingle();

			// removes a singleton TComponent
			// returns true if a component was removed
			template<typename TComponent>
			bool removeSingle();

			// returns an singleton TComponent
			template<typename TComponent>
			TComponent* getSingle();

			// returns an singleton const TComponent
			template<typename TComponent>
			const TComponent* getSingle() const;

			// returns if a singleton Tcomponent exists(fast)
			template<typename TComponent>
			bool hasSingle() const;

			// returns the amount of alive entities
			size_t size() const;
			// returns the amount of dead and alive entities.
			// dead entities are pending recycling
			size_t unadjustedSize() const;
			// returns true if an entity with entityId exists and is alive
			bool isValid(ECS::EntityId entityId) const;

			// Converts a TComponent type into a numerical identifier.
			template<typename TComponent>
			ComponentId getComponentId() const;

			// returns true if the entity will be destroyed when handleDeferredDestroy is called
			bool isMarkedForDestroy(ECS::EntityId entityId) const;
			
			// destroy all entities marked for destroy
			void handleDeferredDestroy();

		private:
			ECS::EntityId m_singletonId;
			EntityContainer m_entityContainer;
		};

		template<typename TComponent, typename... TArgs>
		inline TComponent* Registry::add(ECS::EntityId entityId, TArgs&&... args)
		{
			const ComponentId componentId = m_entityContainer.getComponentId<TComponent>();

			void* buffer = m_entityContainer.addComponent(entityId, componentId, sizeof(TComponent));
			if (buffer == nullptr)
			{
				return nullptr;
			}

			// this is a placement new
			TComponent* component = new (buffer) TComponent(args...);
			return component;
		}

		template<typename ...TComponents>
		inline auto Registry::addMany(ECS::EntityId entityId)
		{
			return std::make_tuple(add<TComponents>(entityId)...);
		}

		template<typename ...TComponents>
		inline auto Registry::removeMany(ECS::EntityId entityId)
		{
			return std::make_tuple(remove<TComponents>(entityId)...);
		}

		template<typename TComponent>
		inline TComponent* Registry::get(ECS::EntityId entityId)
		{
			const ComponentId componentId = m_entityContainer.getComponentId<TComponent>();
			return static_cast<TComponent*>(m_entityContainer.getComponent(entityId, componentId));
		}

		template<typename TComponent>
		inline bool Registry::has(ECS::EntityId entityId) const
		{
			const ComponentId componentId = m_entityContainer.getComponentId<TComponent>();
			return m_entityContainer.hasComponent(entityId, componentId);
		}

		template<typename TComponent>
		inline const TComponent* Registry::get(ECS::EntityId entityId) const
		{
			const ComponentId componentId = m_entityContainer.getComponentId<TComponent>();
			return static_cast<const TComponent*>(m_entityContainer.getComponent(entityId, componentId));
		}

		template<typename... TComponents>
		inline auto Registry::getMany(ECS::EntityId entityId)
		{
			return std::make_tuple(get<TComponents>(entityId)...);
		}

		template<typename... TComponents>
		inline auto Registry::getMany(ECS::EntityId entityId) const
		{
			return std::make_tuple(get<TComponents>(entityId)...);
		}

		template<typename TComponent>
		inline bool Registry::remove(ECS::EntityId entityId)
		{
			const ComponentId componentId = m_entityContainer.getComponentId<TComponent>();
			if (void* data = m_entityContainer.removeComponent(entityId, componentId))
			{
				TComponent* component = static_cast<TComponent*>(data);
				component->~TComponent();
				return true;
			}
			return false;
		}

		template<typename TComponent>
		inline TComponent* Registry::addSingle()
		{
			return add<TComponent>(m_singletonId);
		}

		template<typename TComponent>
		inline bool Registry::removeSingle()
		{
			return remove<TComponent>(m_singletonId);
		}

		template<typename TComponent>
		inline TComponent* Registry::getSingle()
		{
			return get<TComponent>(m_singletonId);
		}

		template<typename TComponent>
		inline const TComponent* Registry::getSingle() const
		{
			return get<TComponent>(m_singletonId);
		}

		template<typename TComponent>
		inline bool Registry::hasSingle() const
		{
			return has<TComponent>(m_singletonId);
		}

		template<typename TComponent>
		inline ComponentId Registry::getComponentId() const
		{
			return m_entityContainer.getComponentId<TComponent>();
		}

		inline Registry::Registry()
		{
			m_singletonId = create();
		}

		inline Registry::~Registry()
		{
			destroy(m_singletonId);
		}

		inline ECS::EntityId Registry::create()
		{
			return m_entityContainer.create();
		}

		inline bool Registry::destroy(ECS::EntityId entityId)
		{
			return m_entityContainer.destroy(entityId);
		}

		inline void Registry::deferDestroy(ECS::EntityId entityId)
		{
			m_entityContainer.deferDestroy(entityId);
		}

		inline const Entity* Registry::getEntity(ECS::EntityId entityId) const
		{
			return m_entityContainer.getEntity(entityId);
		}

		inline size_t Registry::size() const
		{
			return m_entityContainer.size();
		}

		inline size_t Registry::unadjustedSize() const
		{
			return m_entityContainer.unadjustedSize();
		}

		inline bool Registry::isValid(ECS::EntityId entityId) const
		{
			return m_entityContainer.isValid(entityId);
		}

		inline bool Registry::isMarkedForDestroy(ECS::EntityId entityId) const
		{
			return m_entityContainer.isMarkedForDestroy(entityId);
		}

		inline void Registry::handleDeferredDestroy()
		{
			m_entityContainer.handleDeferredDestroy();
		}
	}
}
