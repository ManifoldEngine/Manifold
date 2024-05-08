#pragma once

#include "ECS.h"
#include "EntityContainer.h"
#include "Entity.h"
#include <Events/Event.h>

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

			DECLARE_EVENT(EntityEvent, Registry& /*registry*/, ECS::EntityId /*entityId*/);
			DECLARE_EVENT(EntityComponentEvent, Registry& /*registry*/, ECS::EntityId /*entityId*/, ComponentId /*componentId*/);

			EntityEvent onEntityCreated;
			EntityEvent onBeforeEntityDestroyed;
			EntityEvent onEntityDestroyed;
			EntityComponentEvent onComponentAdded;
			EntityComponentEvent onComponentRemoved;

			Registry();
			~Registry();

			ECS::EntityId getSingletonId() const { return m_singletonId; }

			ECS::EntityId create();
			bool destroy(ECS::EntityId entityId);
			const Entity* getEntity(ECS::EntityId entityId) const;

			template<typename TComponent>
			TComponent* add(ECS::EntityId entityId);

			template<typename TComponent>
			bool remove(ECS::EntityId entityId);

			template<typename TComponent>
			TComponent* get(ECS::EntityId entityId);

			template<typename TComponent>
			const TComponent* get(ECS::EntityId entityId) const;

			template<typename TComponent>
			bool has(ECS::EntityId entityId) const;

			template<typename TComponent>
			TComponent* addSingle();

			template<typename TComponent>
			bool removeSingle();

			template<typename TComponent>
			TComponent* getSingle();

			template<typename TComponent>
			const TComponent* getSingle() const;

			template<typename TComponent>
			bool hasSingle() const;

			size_t size() const;
			size_t unadjustedSize() const;
			bool isValid(ECS::EntityId entityId) const;

			// Converts a TComponent type into a numerical identifier.
			template<typename TComponent>
			ComponentId getComponentId() const;

		private:
			ECS::EntityId m_singletonId;
			EntityContainer m_entityContainer;
		};

		template<typename TComponent>
		inline TComponent* Registry::add(ECS::EntityId entityId)
		{
			const ComponentId componentId = m_entityContainer.getComponentId(typeid(TComponent));

			void* buffer = m_entityContainer.addComponent(entityId, componentId, sizeof(TComponent));
			if (buffer == nullptr)
			{
				return nullptr;
			}

			// this is a placement new
			TComponent* component = new (buffer) TComponent();
			onComponentAdded.broadcast(*this, entityId, componentId);
			return component;
		}

		template<typename TComponent>
		inline TComponent* Registry::get(ECS::EntityId entityId)
		{
			const ComponentId componentId = m_entityContainer.getComponentId(typeid(TComponent));
			return static_cast<TComponent*>(m_entityContainer.getComponent(entityId, componentId));
		}

		template<typename TComponent>
		inline bool Registry::has(ECS::EntityId entityId) const
		{
			const ComponentId componentId = m_entityContainer.getComponentId(typeid(TComponent));
			return m_entityContainer.hasComponent(entityId, componentId);
		}

		template<typename TComponent>
		inline const TComponent* Registry::get(ECS::EntityId entityId) const
		{
			const ComponentId componentId = m_entityContainer.getComponentId(typeid(TComponent));
			return static_cast<const TComponent*>(m_entityContainer.getComponent(entityId, componentId));;
		}

		template<typename TComponent>
		inline bool Registry::remove(ECS::EntityId entityId)
		{
			const ComponentId componentId = m_entityContainer.getComponentId(typeid(TComponent));
			if (m_entityContainer.removeComponent(entityId, componentId))
			{
				onComponentRemoved.broadcast(*this, entityId, componentId);
				return true;
			}
			return false;
		}

		template<typename TComponent>
		inline ComponentId Registry::getComponentId() const
		{
			return m_entityContainer.getComponentId(typeid(TComponent));
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
			const ECS::EntityId entityId = m_entityContainer.create();
			onEntityCreated.broadcast(*this, entityId);
			return entityId;
		}

		inline bool Registry::destroy(ECS::EntityId entityId)
		{
			onBeforeEntityDestroyed.broadcast(*this, entityId);
			if (m_entityContainer.destroy(entityId))
			{
				onEntityDestroyed.broadcast(*this, entityId);
				return true;
			}
			return false;
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
	}
}
