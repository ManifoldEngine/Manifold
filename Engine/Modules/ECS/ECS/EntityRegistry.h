#pragma once

#include "ECS.h"
#include "Internals/EntityContainer.h"
#include "Entity.h"
#include <Events/Event.h>

namespace ECSEngine
{
	/*
	 * Holds a collection of entity and manage their component's memory.
	 */
	class EntityRegistry
	{
#define INITIAL_COMPONENT_COUNT 1000
	public:
		template<typename ...TComponents>
		friend class RegistryView;

		DECLARE_EVENT(EntityEvent, EntityRegistry& /*registry*/, EntityId /*entityId*/);
		DECLARE_EVENT(EntityComponentEvent, EntityRegistry& /*registry*/, EntityId /*entityId*/, ComponentId /*componentId*/);

		EntityEvent onEntityCreated;
		EntityEvent onEntityDestroyed;
		EntityComponentEvent onComponentAdded;
		EntityComponentEvent onComponentRemoved;

		EntityRegistry();
		~EntityRegistry();

		EntityId create();
		bool destroy(EntityId entityId);
		const Entity* getEntity(EntityId entityId) const;
		
		template<typename TComponent>
		TComponent* addComponent(EntityId entityId);

		template<typename TComponent>
		TComponent* getComponent(EntityId entityId);

		template<typename TComponent>
		bool hasComponent(EntityId entityId) const;
		
		template<typename TComponent>
		const TComponent* getComponent(EntityId entityId) const;
		
		template<typename TComponent>
		bool removeComponent(EntityId entityId);

		size_t size() const;
		
		bool isValid(EntityId entityId) const;

		// Converts a TComponent type into a numerical identifier.
		template<typename TComponent>
		ComponentId getComponentId() const;

		// Resets the component id counter. This is mainly for easier testing.
		static void resetComponentIds();

	private:
		static ComponentId s_componentCounter;

		IEntityContainer* m_entityContainer;
	};

	template<typename TComponent>
	inline TComponent* EntityRegistry::addComponent(EntityId entityId)
	{
		const ComponentId componentId = m_entityContainer->getComponentId(typeid(TComponent));

		void* buffer = m_entityContainer->addComponent(entityId, componentId, sizeof(TComponent));
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
	inline TComponent* EntityRegistry::getComponent(EntityId entityId)
	{
		const ComponentId componentId = m_entityContainer->getComponentId(typeid(TComponent));
		return static_cast<TComponent*>(m_entityContainer->getComponent(entityId, componentId));
	}

	template<typename TComponent>
	inline bool EntityRegistry::hasComponent(EntityId entityId) const
	{
		const ComponentId componentId = m_entityContainer->getComponentId(typeid(TComponent));
		return m_entityContainer->hasComponent(entityId, componentId);
	}

	template<typename TComponent>
	inline const TComponent* EntityRegistry::getComponent(EntityId entityId) const
	{
		const ComponentId componentId = m_entityContainer->getComponentId(typeid(TComponent));
		return static_cast<const TComponent*>(m_entityContainer->getComponent(entityId, componentId));;
	}

	template<typename TComponent>
	inline bool EntityRegistry::removeComponent(EntityId entityId)
	{
		const ComponentId componentId = m_entityContainer->getComponentId(typeid(TComponent));
		if (m_entityContainer->removeComponent(entityId, componentId))
		{
			onComponentRemoved.broadcast(*this, entityId, componentId);
			return true;
		}
		return false;
	}

	template<typename TComponent>
	inline ComponentId EntityRegistry::getComponentId() const
	{
		return m_entityContainer->getComponentId(typeid(TComponent));
	}
}
