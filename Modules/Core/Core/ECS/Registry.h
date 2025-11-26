#pragma once

#include "Container/EntityContainer.h"
#include "Entity.h"
#include <iostream>
#include <tuple>

#if MANI_DEBUG
#include <Container/TypeInfoContainer.h>	
#endif

namespace Mani
{
	namespace ECS
	{
		/*
		 * Holds a collection of entity and manage their component's memory.
		 */
		class Registry
		{
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

			// returns an entity object
			const Entity* getEntityAt(ECS::Index index) const;

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

			// returns an entity's TComponent
			template<typename TComponent>
			TComponent& getRef(ECS::EntityId entityId);

			// returns an entity's const TComponent
			template<typename TComponent>
			const TComponent& getRef(ECS::EntityId entityId) const;

			// returns an entity's TComponent in a tuple
			template<typename ...TComponents>
			auto getMany(ECS::EntityId entityId);

			// returns an entity's const TComponent in a tuple
			template<typename ...TComponents>
			auto getMany(ECS::EntityId entityId) const;

			// returns true if an entity has a component (fast)
			template<typename ...TComponents>
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
			ECS::Index count() const;
			// returns the amount of dead and alive entities.
			// dead entities are pending recycling
			ECS::Index unadjustedCount() const;
			// returns true if an entity with entityId exists and is alive
			bool isValid(ECS::EntityId entityId) const;
			// returns true if an entity with index exists and is alive
			bool isValidIndex(ECS::Index index) const;

			// returns true if the entity will be destroyed when handleDeferredDestroy is called
			bool isMarkedForDestroy(ECS::EntityId entityId) const;
			
			// destroy all entities marked for destroy
			void handleDeferredDestroy();

			// Converts a TComponent type into a numerical identifier.
			template<typename T>
			ComponentId getComponentId() const;
		private:
			ECS::EntityId m_singletonId;
			EntityContainer m_entityContainer;

			inline static ComponentId TYPE_ID_SEQUENCE = 0;
			template<typename T>
			inline static ComponentId TYPE_ID = TYPE_ID_SEQUENCE++;

#if MANI_DEBUG
		public:
			const TypeInfo& getTypeInfo(ComponentId componentId) const
			{
				return m_typeInfoContainer.getTypeInfo(componentId);
			}

			List<TypeInfo> getTypeInfo(const Entity& entity) const
			{
				return m_typeInfoContainer.getTypeInfo(entity);
			}

			ComponentId reflect(const std::string_view& name) const
			{
				return m_typeInfoContainer.reflect(name);
			}

		private:
			mutable TypeInfoContainer m_typeInfoContainer;
#endif
		};

		template<typename TComponent, typename... TArgs>
		inline TComponent* Registry::add(ECS::EntityId entityId, TArgs&&... args)
		{
			const ComponentId componentId = getComponentId<TComponent>();

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
			const ComponentId componentId = getComponentId<TComponent>();
			return static_cast<TComponent*>(m_entityContainer.getComponent(entityId, componentId));
		}

		template<typename ...TComponents>
		inline bool Registry::has(ECS::EntityId entityId) const
		{
			auto f = [&]<typename TComponent>() -> bool
			{
				const ComponentId componentId = getComponentId<TComponent>();
				return m_entityContainer.hasComponent(entityId, componentId);
			};
			return (f.template operator()<TComponents>() && ...);
		}

		template<typename TComponent>
		inline const TComponent* Registry::get(ECS::EntityId entityId) const
		{
			const ComponentId componentId = getComponentId<TComponent>();
			return static_cast<const TComponent*>(m_entityContainer.getComponent(entityId, componentId));
		}

		// returns an entity's TComponent
		template<typename TComponent>
		inline TComponent& Registry::getRef(ECS::EntityId entityId)
		{
			TComponent* component = get<TComponent>(entityId);
			MANI_ASSERT(component != nullptr, "Trying to get a component that an entity doesn't have");
			return *component;
		}

		// returns an entity's const TComponent
		template<typename TComponent>
		const TComponent& Registry::getRef(ECS::EntityId entityId) const
		{
			const TComponent* component = get<TComponent>(entityId);
			MANI_ASSERT(component != nullptr, "Trying to get a component that an entity doesn't have");
			return *component;
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
			const ComponentId componentId = getComponentId<TComponent>();
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

		template<typename T>
		inline ComponentId Registry::getComponentId() const
		{
#if MANI_DEBUG
			m_typeInfoContainer.registerComponent<T>(TYPE_ID<T>);
#endif
			return TYPE_ID<T>;
		}

		inline Registry::Registry()
		{
			m_singletonId = create();
		}

		inline Registry::~Registry()
		{
			m_entityContainer.destroy(m_singletonId);
		}

		inline ECS::EntityId Registry::create()
		{
			return m_entityContainer.create();
		}

		inline bool Registry::destroy(ECS::EntityId entityId)
		{
			MANI_ASSERT(entityId != m_singletonId, "Something is attempting to destroy the singleton entity.");
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

		inline const Entity* Registry::getEntityAt(ECS::Index index) const
		{
			return m_entityContainer.getEntityAt(index);
		}

		inline ECS::Index Registry::count() const
		{
			return m_entityContainer.count();
		}

		inline ECS::Index Registry::unadjustedCount() const
		{
			return m_entityContainer.unadjustedCount();
		}

		inline bool Registry::isValid(ECS::EntityId entityId) const
		{
			return m_entityContainer.isValid(entityId);
		}

		inline bool Registry::isValidIndex(ECS::Index index) const
		{
			return m_entityContainer.isValidIndex(index);
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
