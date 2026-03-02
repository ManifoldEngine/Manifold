#pragma once

#include <Core/ECS/Entity.h>
#include <Core/Containers/List.h>
#include <Core/Containers/SparseSet.h>
#include <limits>
#include <mutex>
#include <memory>

namespace Mani
{
	namespace ECS
	{
		inline constexpr SizeT MAX_ENTITY_COUNT = std::numeric_limits<SizeT>::max();

		class Registry
		{
		public:
			Registry()
			{
				m_singletonId = create();
			}

			~Registry()
			{
				destroy(m_singletonId);
			}

			Registry(const Registry&) = delete;
			Registry(Registry&) = delete;

			// creates an entity id
			[[nodiscard]] EntityId create()
			{
				if (m_entities.count() >= MAX_ENTITY_COUNT && m_recyclableIndices.isEmpty())
				{
					MANI_ASSERT(false, "Reached maximum entitty capacity");
					return ECS::INVALID_ID;
				}

				Entity* entity = nullptr;
				if (!m_recyclableIndices.isEmpty())
				{
					ECS::Index index = m_recyclableIndices.pop();
					entity = &m_entities[index];
					const Version newVersion = entity->getVersion() + 1;
					entity->setVersion(newVersion);

#if MANI_DEBUG
					if (newVersion >= ECS::MAX_VERSION)
					{
						MANI_LOG_WARNING(LogCore, "Entity version loopback. This is not necessarily a problem. If I were you, I'd hope that nothing is keeping a reference to a prior version of that entity :)");
					}
#endif
				}
				else
				{
					ECS::Index index = static_cast<ECS::Index>(m_entities.count());
					m_entities.add(Entity());
					entity = &m_entities.last();
					entity->setIndex(index);
					entity->setVersion(0);
				}

				MANI_ASSERT(entity != nullptr, "Null entity found in the entity list");
				entity->isAlive = true;
				return entity->getId();
			}

			// destroys an entity and its components
			bool destroy(EntityId entityId)
			{
				if (!isValid(entityId))
				{
					return false;
				}

				const ECS::Index index = ECS::toIndex(entityId);
				Entity& entity = m_entities[index];

				// delete its components
				for (ComponentId componentId = 0; componentId < m_componentPools.count(); componentId++)
				{
					if (entity.hasComponent(componentId))
					{
						m_componentPools[componentId]->remove(index);
					}
				}

				// prepare it for recycling
				entity.isAlive = false;
				entity.resetComponentBits();
				m_recyclableIndices.add(index);

				return true;
			}

			// destroys at the end of the tick.
			void deferDestroy(EntityId entityId)
			{
				std::scoped_lock<std::mutex> lock(m_markedForDestroyMutex);
				m_markedForDestroy.addUnique(entityId);
			}

			// returns true if an entity with entityId exists and is alive
			[[nodiscard]] bool isValid(EntityId entityId) const
			{
				if (entityId == ECS::INVALID_ID)
				{
					return false;
				}

				const ECS::Index index = ECS::toIndex(entityId);
				if (!m_entities.isValid(index))
				{
					return false;
				}

				const Entity& entity = m_entities[index];
				return entity.getVersion() == ECS::toVersion(entityId) && entity.isAlive;
			}

			// returs an entity object
			[[nodiscard]] const Entity* getEntity(EntityId entityId) const
			{
				if (!isValid(entityId))
				{
					return nullptr;
				}
				return &m_entities[ECS::toIndex(entityId)];
			}

			// returns an entity object
			[[nodiscard]] const Entity* getEntityAt(ECS::Index index) const
			{
				if (!m_entities.isValid(index))
				{
					return nullptr;
				}
				return &m_entities[index];
			}

			// Converts a T component type into a numerical identifier.
			template<typename T>
			[[nodiscard]] ComponentId getComponentId() const
			{
				return TYPE_ID<T>;
			}

			// adds a T component to an entity
			// returns the added component
			template<typename T>
			T& add(EntityId entityId, T&& value = T{})
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				// get or initialize needed component pool
				SparseSet<T>& componentPool = getOrAddComponentPool<T>();
				const ComponentId componentId = getComponentId<T>();

				const ECS::Index index = ECS::toIndex(entityId);
				// set the component's bit on the designated entity
				Entity& entity = m_entities[index];
				MANI_ASSERT(!entity.hasComponent(componentId), "Trying to overwrite an existing component");
				entity.setComponentBit(componentId);

				// store the component and return it
				T& component = componentPool.set(index);
				component = std::move(value);
				return component;
			}

			// adds all T components to an entity
			// returns the added components
			template<typename ...Ts>
			auto addMany(EntityId entityId)
			{
				return std::make_tuple(add<Ts>(entityId)...);
			}

			// adds a singleton T component
			// returns the added component
			template<typename T>
			T& addSingle()
			{
				return add<T>(m_singletonId);
			}

			// removes a T components to an entity
			// returns true if a component was removed
			template<typename T>
			bool remove(EntityId entityId)
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				const ECS::Index index = ECS::toIndex(entityId);
				SparseSet<T>* componentPool = getComponentPoolPtr<T>();
				MANI_ASSERT(componentPool != nullptr, COMPONENT_POOL_NOT_FOUND_MESSAGE);
				if (!componentPool->remove(index))
				{
					return false;
				}

				const ComponentId componentId = getComponentId<T>();
				Entity& entity = m_entities[index];
				entity.resetComponentBit(componentId);
				return true;
			}

			// removes all T components to an entity
			// returns true if a component was removed
			template<typename ...Ts>
			auto removeMany(EntityId entityId)
			{
				return std::make_tuple(remove<Ts>(entityId)...);
			}

			// removes a singleton T component
			// returns true if a component was removed
			template<typename T>
			bool removeSingle()
			{
				return remove<T>(m_singletonId);
			}

			// returns an entity's T component as a reference
			template<typename T>
			[[nodiscard]] T& get(EntityId entityId)
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);
				SparseSet<T>* componentPool = getComponentPoolPtr<T>();
				MANI_ASSERT(componentPool != nullptr, COMPONENT_POOL_NOT_FOUND_MESSAGE);
				T* component = componentPool->getPtr(ECS::toIndex(entityId));
				MANI_ASSERT(component != nullptr, COMPONENT_NOT_FOUND_MESSAGE, entityId);
				return *component;
			}

			// returns an entity's T component as a const reference
			template<typename T>
			[[nodiscard]] const T& get(EntityId entityId) const
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);
				const SparseSet<T>* componentPool = getComponentPoolPtr<T>();
				MANI_ASSERT(componentPool != nullptr, COMPONENT_POOL_NOT_FOUND_MESSAGE);
				const T* component = componentPool->getPtr(ECS::toIndex(entityId));
				MANI_ASSERT(component != nullptr, COMPONENT_NOT_FOUND_MESSAGE, entityId);
				return *component;
			}

			// returns an entity's T component as a pointer
			template<typename T>
			[[nodiscard]] T* getPtr(EntityId entityId)
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);
				SparseSet<T>* componentPool = getComponentPoolPtr<T>();
				MANI_ASSERT(componentPool != nullptr, COMPONENT_POOL_NOT_FOUND_MESSAGE);
				return componentPool->getPtr(ECS::toIndex(entityId));
			}

			// returns an entity's T component as a const pointer
			template<typename T>
			[[nodiscard]] const T* getPtr(EntityId entityId) const
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);
				const SparseSet<T>* componentPool = getComponentPoolPtr<T>();
				MANI_ASSERT(componentPool != nullptr, COMPONENT_POOL_NOT_FOUND_MESSAGE);
				return componentPool->getPtr(ECS::toIndex(entityId));
			}

			// returns the singleton's T component as a reference
			template<typename T>
			[[nodiscard]] T& getSingle()
			{
				return get<T>(m_singletonId);
			}

			// returns the singleton's T component as a const reference
			template<typename T>
			[[nodiscard]] const T& getSingle() const
			{
				return get<T>(m_singletonId);
			}

			// returns the singleton's T component as a pointer
			template<typename T>
			[[nodiscard]] T* getSinglePtr()
			{
				return getPtr<T>(m_singletonId);
			}

			// returns the singleton's T component as a reference
			template<typename T>
			[[nodiscard]] const T* getSinglePtr() const
			{
				return getPtr<T>(m_singletonId);
			}

			// returns true if entity matches the Ts component
			template<typename ...Ts>
			[[nodiscard]] bool has(EntityId entityId) const
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);
				const Entity& entity = m_entities[ECS::toIndex(entityId)];
				return entity.hasComponents(getMask<Ts...>());
			}

			// returns true if singleton entity matches the Ts component
			template<typename ...Ts>
			[[nodiscard]] bool hasSingle() const
			{
				return has<Ts...>(m_singletonId);
			}

			// returns the component mask for Ts components
			template<typename ...Ts>
			[[nodiscard]] ComponentMask getMask() const
			{
				ComponentMask mask;
				(mask.set(getComponentId<Ts>()), ...);
				return mask;
			}

			// returns the count alive entities
			[[nodiscard]] SizeT count() const
			{
				return m_entities.count() - m_recyclableIndices.count();
			}

			// destroy all entities marked for destroy
			void handleDeferredDestroy()
			{
				std::scoped_lock<std::mutex> lock(m_markedForDestroyMutex);
				for (const auto entityId : m_markedForDestroy)
				{
					destroy(entityId);
				}
				m_markedForDestroy.clear();
			}

			template<typename T>
			[[nodiscard]] const SparseSet<T>* getComponentPoolPtr() const
			{
				const ComponentId componentId = getComponentId<T>();
				if (!m_componentPools.isValid(componentId))
				{
					return nullptr;
				}

				return static_cast<SparseSet<T>*>(m_componentPools[componentId].get());
			}
		
		private:
			template<typename T>
			[[nodiscard]] SparseSet<T>& getOrAddComponentPool()
			{
				const ComponentId componentId = getComponentId<T>();
				if (!m_componentPools.isValid(componentId))
				{
					m_componentPools.resize(componentId + 1);
					MANI_ASSERT(m_componentPools.count() <= ECS::MAX_COMPONENTS, "MAX_COMPONENTS has been reached");
				}

				if (m_componentPools[componentId] == nullptr)
				{
					m_componentPools[componentId] = std::make_unique<SparseSet<T>>();
				}

				SparseSet<T>* componentPool = static_cast<SparseSet<T>*>(m_componentPools[componentId].get());
				MANI_ASSERT(componentPool != nullptr, "Obtained null component pool after component pools allocation");
				return *componentPool;
			}

			template<typename T>
			[[nodiscard]] SparseSet<T>* getComponentPoolPtr()
			{
				const ComponentId componentId = getComponentId<T>();
				if (!m_componentPools.isValid(componentId))
				{
					return nullptr;
				}

				return static_cast<SparseSet<T>*>(m_componentPools[componentId].get());
			}

			EntityId m_singletonId = ECS::INVALID_ID;
			Mani::List<Entity> m_entities;
			Mani::List<Index> m_recyclableIndices;

			Mani::List<EntityId> m_markedForDestroy;
			mutable std::mutex m_markedForDestroyMutex;

			Mani::List<std::unique_ptr<ISparseSet>> m_componentPools;

			inline static ComponentId TYPE_ID_SEQUENCE = 0;
			template<typename T>
			inline static ComponentId TYPE_ID = TYPE_ID_SEQUENCE++;

			inline static constexpr std::string_view INVALID_ENTITY_MESSAGE = "Entity {} is invalid";
			inline static constexpr std::string_view COMPONENT_POOL_NOT_FOUND_MESSAGE = "Component pool is not registered for component";
			inline static constexpr std::string_view COMPONENT_NOT_FOUND_MESSAGE = "Entity {} does not have component";
		};
	}
}