#pragma once

#include <Core/ECS/Entity.h>
#include <Core/ECS/Archetype.h>
#include <Core/ECS/Ref.h>

#include <Core/Containers/List.h>
#include <Core/Containers/SparseArray.h>
#include <Core/ManiTraits.h>
#include <Core/ManiTypes.h>

#include <limits> 
#include <mutex>
#include <thread>

#if MANI_ASSERT_ENABLED
#include <ManiZ/Reflection.h>
#endif

namespace Mani
{
	namespace ECS
	{
		inline constexpr SizeT MAX_ENTITY_COUNT = std::numeric_limits<SizeT>::max();
		inline constexpr SizeT INITIAL_COMPONENT_CAPACITY = 1000;
		inline constexpr SizeT INITIAL_ARCHETYPE_CAPACITY = 256;
		inline constexpr SizeT PINNED_COMPONENTS_CAPACITY = 512;

		/**
		 * Registry of entities and components. 
		 *	A few notes about the different storage strategy available:
		 *		Use add & remove for long lasting components, which should be the vast majority of
		 *		use cases. These components offer fast iteration at the cost of expensive add/remove
		 * 
		 *		Alternatively, use pinned component for tagging as they're quite cheap to add and remove.
		 *		Also, pinned components offer stable reference and can be used to share resources accross thread.	
		 */
		class Registry
		{
			template<class TRegistry, class TArchetype, typename ...Ts>
			requires(DerivedFrom<TRegistry, Registry>&& DerivedFrom<TArchetype, Archetype>)
			friend class BaseView;

			template<class TRegistry, typename ...Ts>
			requires(DerivedFrom<TRegistry, Registry>)
			friend class BasePinnedView;
			
			template<typename T>
			using Ref = ECS::ComponentRef<T>;

			template<typename T>
			using ComponentPool = Mani::SparseArray<T, PINNED_COMPONENTS_CAPACITY, ECS::EntityId>;
			using IComponentPool = Mani::ISparseArray<PINNED_COMPONENTS_CAPACITY>;

		public:

			Registry()
			{
				m_archetypes.reserve(INITIAL_ARCHETYPE_CAPACITY);
				m_threadId = Mani::thisThreadId();
				m_singletonId = create();
			}

			~Registry()
			{
				for (auto it = m_entities.rbegin(); it != m_entities.rend(); ++it)
				{
					ECS::Entity& entity = *it;
					destroy_internal(entity.getId());
				}
			}

			Registry(const Registry&) = delete;
			Registry(Registry&) = delete;

			// creates an entity id
			[[nodiscard]] ECS::EntityId create()
			{
				ASSERT_SAME_THREAD();

				if (m_entities.count() >= MAX_ENTITY_COUNT && m_recyclableIndices.isEmpty())
				{
					MANI_ASSERT(false, "Reached maximum entitty capacity");
					return ECS::INVALID_ID;
				}

				ECS::Entity* entity = nullptr;
				if (!m_recyclableIndices.isEmpty())
				{
					ECS::Index index = m_recyclableIndices.pop();
					entity = &m_entities[index];

#if MANI_DEBUG
					const ECS::Version version = entity->version;
					if (version >= ECS::MAX_VERSION)
					{
						MANI_LOG_WARNING(LogCore, "ECS::Entity version loopback. This is not necessarily a problem. If I were you, I'd hope that nothing is keeping a reference to a prior version of that entity :)");
					}
#endif
					entity->version += 1;
				}
				else
				{
					ECS::Index index = static_cast<ECS::Index>(m_entities.count());
					m_entities.add(ECS::Entity(index));
					entity = &m_entities.last();
					
					entity->version = 0;
				}

				MANI_ASSERT(entity != nullptr, "Null entity found in the entity list");
				entity->isAlive = true;
				return entity->getId();
			}

			// destroys an entity and its components
			bool destroy(ECS::EntityId entityId)
			{
				MANI_ASSERT(entityId != m_singletonId, "Trying to destroy the singleton entity");
				return destroy_internal(entityId);
			}

			// destroys at the end of the tick.
			void deferDestroy(ECS::EntityId entityId)
			{
				std::scoped_lock<std::mutex> lock(m_markedForDestroyMutex);
				m_markedForDestroy.addUnique(entityId);
			}

			// returns true if an entity with entityId exists and is alive
			[[nodiscard]] bool isValid(ECS::EntityId entityId) const
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

				const ECS::Entity& entity = m_entities[index];
				return entity.version == ECS::toVersion(entityId) && entity.isAlive;
			}

			// returs an entity object
			[[nodiscard]] const ECS::Entity* getEntity(ECS::EntityId entityId) const
			{
				if (!isValid(entityId))
				{
					return nullptr;
				}
				return &m_entities[ECS::toIndex(entityId)];
			}

			// returns an entity object
			[[nodiscard]] const ECS::Entity* getEntityAt(ECS::Index index) const
			{
				if (!m_entities.isValid(index))
				{
					return nullptr;
				}
				return &m_entities[index];
			}

			// Converts a T component type into a numerical identifier.
			template<typename T>
			[[nodiscard]] ECS::ComponentId getComponentId() const
			{
				using Component = Bare<T>;
				return TYPE_ID<Component>;
			}

			[[nodiscard]] SizeT getComponentCount() const
			{
				return TYPE_ID_SEQUENCE;
			}

			// adds a T component to an entity
			// returns the added component
			template<typename T, typename ...TArgs>
			requires(std::is_move_constructible_v<T>)
			Ref<T> add(ECS::EntityId entityId, TArgs&&... args)
			{
				ASSERT_UNLOCKED();
				ASSERT_SAME_THREAD();
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				const ECS::ComponentId componentId = getAndRegisterComponentId<T>();
				ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];

				MANI_ASSERT(!entity.components.test(componentId), "Trying to overwrite an existing component");
				MANI_ASSERT(!entity.pinned.test(componentId), "Entity already has a pinned component of type {}", ManiZ::RFL::getTypeName<T>());
				
				const ECS::ComponentMask oldMask = entity.components;
				entity.components.set(componentId, true);
				const ECS::ComponentMask newMask = entity.components;

				auto [oldArchetype, newArchetype] = getArchetypes<T>(oldMask, newMask);

				newArchetype->add(entityId);
				if (oldArchetype != nullptr)
				{
					Archetype::move(entityId, *oldArchetype, *newArchetype);
					oldArchetype->removeSwap(entityId);
				}

				T* component = newArchetype->get<T>(entityId, componentId);
				component = new (component) T(std::forward<TArgs>(args)...);
				return Ref(component, *newArchetype);
			}

			// adds all T components to an entity
			// returns the added components
			template<typename ...Ts>
			requires(std::is_move_constructible_v<Ts> && ...)
			std::tuple<Ref<Ts>...> addMany(ECS::EntityId entityId)
			{
				ASSERT_UNLOCKED();
				ASSERT_SAME_THREAD();
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				const Mani::Array<ECS::ComponentId, sizeof... (Ts)> componentIds = { this->template getAndRegisterComponentId<Ts>()... };
				ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];

#if MANI_ASSERT_ENABLED
				for (const ECS::ComponentId componentId : componentIds)
				{
					MANI_ASSERT(!entity.components.test(componentId), "Trying to overwrite an existing component");
				}
#endif

				const ECS::ComponentMask oldMask = entity.components;
				for (const ECS::ComponentId componentId : componentIds)
				{
					entity.components.set(componentId, true);
				}
				const ECS::ComponentMask newMask = entity.components;

				auto [oldArchetype, newArchetype] = getArchetypes<Ts...>(oldMask, newMask);

				newArchetype->add(entityId);
				if (oldArchetype != nullptr)
				{
					Archetype::move(entityId, *oldArchetype, *newArchetype);
					oldArchetype->removeSwap(entityId);
				}

				auto init = [this, newArchetype, entityId]<typename T>() -> Ref<T>
				{
					T* component = newArchetype->get<T>(entityId, getComponentId<T>());
					component = new (component) T();
					return Ref(component, *newArchetype);
				};
				return std::tuple<Ref<Ts>...>(init.template operator()<Ts>()...);
			}

			// adds a singleton T component
			// returns the added component
			template<typename T>
			requires(std::is_move_constructible_v<T>)
			Ref<T> addSingle()
			{
				return add<T>(m_singletonId);
			}

			// adds a pinned T component
			// pinned components are cheaper to add/remove
			// and their references are stable
			// returns the added component
			template<typename T, typename ...TArgs>
			T& addPinned(ECS::EntityId entityId, TArgs&&... args)
			{
				ASSERT_SAME_THREAD();
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				const ECS::ComponentId componentId = getAndRegisterComponentId<T>();
				const ECS::Index index = ECS::toIndex(entityId);
				ECS::Entity& entity = m_entities[index];

				MANI_ASSERT(!entity.components.test(componentId), "Entity already has a component of type {}", ManiZ::RFL::getTypeName<T>());
				MANI_ASSERT(!entity.pinned.test(componentId), "Trying to overwrite an existing component {}", ManiZ::RFL::getTypeName<T>());

				entity.pinned.set(componentId, true);
				
				ComponentPool<T>& pool = getOrAddPinnedComponentPool<T>();
				pool.set(index, T(std::forward<TArgs>(args)...));
				return pool.get(index);
			}

			template<typename T>
			T& addSinglePinned()
			{
				return addPinned<T>(m_singletonId);
			}

			// removes a T components to an entity
			template<typename T>
			void remove(ECS::EntityId entityId)
			{
				ASSERT_UNLOCKED();
				ASSERT_SAME_THREAD();
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];
				
				const ECS::ComponentId componentId = getComponentId<T>();
				MANI_ASSERT(entity.components.test(componentId), "Entity doesn't have the component {}", ManiZ::RFL::getTypeName<T>());
				
				const ECS::ComponentMask oldMask = entity.components;
				entity.components.set(componentId, false);
				const ECS::ComponentMask newMask = entity.components;

				auto [oldArchetype, newArchetype] = getArchetypes<T>(oldMask, newMask);

				if (newMask.any())
				{
					newArchetype->add(entityId);
				}

				TYPE_DESTRUCTORS.get(componentId)(oldArchetype->getRaw(entityId, componentId));
				if (newMask.any())
				{
					Archetype::move(entityId, *oldArchetype, *newArchetype);
				}

				oldArchetype->removeSwap(entityId);
			}

			// removes all T components to an entity
			template<typename ...Ts>
			void removeMany(ECS::EntityId entityId)
			{
				ASSERT_UNLOCKED();
				ASSERT_SAME_THREAD();
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				const Mani::Array<ECS::ComponentId, sizeof... (Ts)> componentIds = { this->template getAndRegisterComponentId<Ts>()... };
				ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];

#if MANI_ASSERT_ENABLED
				for (const ECS::ComponentId componentId : componentIds)
				{
					MANI_ASSERT(entity.components.test(componentId), "Entity doesn't have the component you're trying to remove");
				}
#endif
				const ECS::ComponentMask oldMask = entity.components;
				MANI_ASSERT(oldMask.any(), "attempting to remove components from an entity that doesn't have any");
				for (const ECS::ComponentId componentId : componentIds)
				{
					entity.components.set(componentId, false);
				}
				const ECS::ComponentMask newMask = entity.components;

				auto [oldArchetype, newArchetype] = getArchetypes<Ts...>(oldMask, newMask);
				MANI_ASSERT(oldArchetype != nullptr, "Could not find old archetype when remove, something went wrong.");
				
				if (newMask.any())
				{
					newArchetype->add(entityId);
				}

				for (const ECS::ComponentId componentId : componentIds)
				{
					TYPE_DESTRUCTORS.get(componentId)(oldArchetype->getRaw(entityId, componentId));
				}

				if (newMask.any())
				{
					Archetype::move(entityId, *oldArchetype, *newArchetype);
				}

				oldArchetype->removeSwap(entityId);
			}

			// removes a singleton T component
			template<typename T>
			void removeSingle()
			{
				remove<T>(m_singletonId);
			}

			template<typename T>
			void removePinned(ECS::EntityId entityId)
			{
				ASSERT_SAME_THREAD();
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);

				const ECS::ComponentId componentId = getComponentId<T>();
				const ECS::Index index = ECS::toIndex(entityId);
				ECS::Entity& entity = m_entities[index];

				MANI_ASSERT(entity.pinned.test(componentId), "Entity doesn't have the component {}", ManiZ::RFL::getTypeName<T>());

				entity.pinned.set(componentId, false);

				ComponentPool<T>* pool = getPinnedComponentPoolPtr<T>();
				MANI_ASSERT(pool != nullptr, "Trying to get an unregister component pool for {}", ManiZ::RFL::getTypeName<T>());
				pool->unset(index);
			}

			template<typename T>
			void removeSinglePinned()
			{
				removePinned<T>(m_singletonId);
			}

			// returns an entity's T component as a reference
			template<typename T>
			[[nodiscard]] Ref<T> get(ECS::EntityId entityId)
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);
				const ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];
				MANI_ASSERT(entity.components.test(getComponentId<T>()), COMPONENT_NOT_FOUND_MESSAGE, ManiZ::RFL::getTypeName<T>());
				auto& archetype = m_archetypes.get(entity.components);
				return Ref(archetype->get<T>(entityId, getComponentId<T>()), *archetype);
			}

			// returns an entity's T component as a const reference
			template<typename T>
			[[nodiscard]] Ref<const T> get(ECS::EntityId entityId) const
			{
				MANI_ASSERT(isValid(entityId), INVALID_ENTITY_MESSAGE, entityId);
				const ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];
				MANI_ASSERT(entity.components.test(getComponentId<T>()), COMPONENT_NOT_FOUND_MESSAGE, ManiZ::RFL::getTypeName<T>());
				const auto& archetype = m_archetypes.get(entity.components);
				return Ref<const T>(archetype->get<T>(entityId, getComponentId<T>()), *archetype);
			}

			// returns an entity's T component as a pointer
			template<typename T>
			[[nodiscard]] Ref<T> find(ECS::EntityId entityId)
			{
				if (!isValid(entityId))
				{
					return Ref<T>::INVALID();
				}

				const ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];
				const ECS::ComponentId componentId = getComponentId<T>();
				if (!entity.components.test(componentId))
				{
					return Ref<T>::INVALID();
				}

				auto& archetype = m_archetypes.get(entity.components);
				return Ref(archetype->get<T>(entityId, componentId), *archetype);
			}

			// returns an entity's T component as a const pointer
			template<typename T>
			[[nodiscard]] Ref<const T> find(ECS::EntityId entityId) const
			{
				if (!isValid(entityId))
				{
					return Ref<const T>::INVALID();
				}

				const ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];
				const ECS::ComponentId componentId = getComponentId<T>();
				if (!entity.components.test(componentId))
				{
					return Ref<const T>::INVALID();
				}

				const auto& archetype = m_archetypes.get(entity.components);
				return Ref<const T>(archetype->get<T>(entityId, componentId), *archetype);
			}

			// returns the singleton's T component as a reference
			template<typename T>
			[[nodiscard]] Ref<T> getSingle()
			{
				return get<T>(m_singletonId);
			}

			// returns the singleton's T component as a const reference
			template<typename T>
			[[nodiscard]] Ref<const T> getSingle() const
			{
				return get<T>(m_singletonId);
			}

			// returns the singleton's T component as a pointer
			template<typename T>
			[[nodiscard]] Ref<T> findSingle()
			{
				return find<T>(m_singletonId);
			}

			// returns the singleton's T component as a reference
			template<typename T>
			[[nodiscard]] Ref<const T> findSingle() const
			{
				return find<T>(m_singletonId);
			}

			template<typename T>
			[[nodiscard]] T& getPinned(ECS::EntityId entityId)
			{
				ComponentPool<T>* pool = getPinnedComponentPoolPtr<T>();
				MANI_ASSERT(pool != nullptr, "Trying to get an unregister component pool for {}", ManiZ::RFL::getTypeName<T>());
				return pool->get(ECS::toIndex(entityId));
			}

			template<typename T>
			[[nodiscard]] const T& getPinned(ECS::EntityId entityId) const
			{
				const ComponentPool<T>* pool = getPinnedComponentPoolPtr<T>();
				MANI_ASSERT(pool != nullptr, "Trying to get an unregister component pool for {}", ManiZ::RFL::getTypeName<T>());
				return pool->get(ECS::toIndex(entityId));
			}

			template<typename T>
			[[nodiscard]] T* findPinned(ECS::EntityId entityId)
			{
				if (ComponentPool<T>* pool = getPinnedComponentPoolPtr<T>())
				{
					return pool->getPtr(ECS::toIndex(entityId));
				}
				return nullptr;
			}

			template<typename T>
			[[nodiscard]] const T* findPinned(ECS::EntityId entityId) const
			{
				if (const ComponentPool<T>* pool = getPinnedComponentPoolPtr<T>())
				{
					return pool->getPtr(ECS::toIndex(entityId));
				}
				return nullptr;
			}

			template<typename T>
			[[nodiscard]] T& getSinglePinned()
			{
				return getPinned<T>(m_singletonId);
			}

			template<typename T>
			[[nodiscard]] const T& getSinglePinned() const
			{
				return getPinned<T>(m_singletonId);
			}

			template<typename T>
			[[nodiscard]] T* findSinglePinned()
			{
				return findPinned<T>(m_singletonId);
			}

			template<typename T>
			[[nodiscard]] const T* findSinglePinned() const
			{
				return findPinned<T>(m_singletonId);
			}

			// returns true if entity matches the Ts component
			template<typename ...Ts>
			[[nodiscard]] bool has(ECS::EntityId entityId) const
			{
				if (!isValid(entityId))
				{
					return false;
				}

				const ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];
				return entity.components.contains(getMask<Ts...>());
			}

			// returns true if singleton entity matches the Ts component
			template<typename ...Ts>
			[[nodiscard]] bool hasSingle() const
			{
				return has<Ts...>(m_singletonId);
			}

			// returns true if entity matches the Ts pinned component
			template<typename ...Ts>
			[[nodiscard]] bool hasPinned(ECS::EntityId entityId) const
			{
				if (!isValid(entityId))
				{
					return false;
				}

				const ECS::Entity& entity = m_entities[ECS::toIndex(entityId)];
				return entity.pinned.contains(getMask<Ts...>());
			}

			// returns the component mask for Ts components
			template<typename ...Ts>
			[[nodiscard]] ECS::ComponentMask getMask() const
			{
				ECS::ComponentMask mask;
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

		private:
			bool destroy_internal(ECS::EntityId entityId)
			{
				ASSERT_SAME_THREAD();
				ASSERT_UNLOCKED();

				if (!isValid(entityId))
				{
					return false;
				}

				const ECS::Index index = ECS::toIndex(entityId);
				ECS::Entity& entity = m_entities[index];

				if (auto* archetypePtr = m_archetypes.find(entity.components))
				{
					auto& archetype = *archetypePtr;
					for (const auto& componentId : archetype->getComponentIds())
					{
						TYPE_DESTRUCTORS.get(componentId)(archetype->getRaw(entityId, componentId));
					}
					archetype->removeSwap(entityId);
				}

				// prepare it for recycling
				entity.isAlive = false;
				entity.components.reset();
				entity.pinned.reset();
				m_recyclableIndices.add(index);

				return true;
			}

			template<typename T>
			[[nodiscard]] ECS::ComponentId getAndRegisterComponentId()
			{
				const ECS::ComponentId componentId = getComponentId<T>();
				if (!TYPE_SIZES.has(componentId))
				{
					TYPE_SIZES.add(componentId, sizeof(T));
					TYPE_DESTRUCTORS.add(componentId, [](const void* o) { static_cast<const T*>(o)->~T(); });
				}
				return componentId;
			}

			Mani::Map<ECS::ComponentId, SizeT> makeLayout(const Mani::List<ECS::ComponentId>& componentIds)
			{
				Mani::Map<ECS::ComponentId, SizeT> layout;
				for (ECS::ComponentId id : componentIds)
				{
					layout.add(id, TYPE_SIZES.get(id));
				}
				return layout;
			}

			template<typename ...Ts>
			std::tuple<Archetype*, Archetype*> getArchetypes(ECS::ComponentMask oldMask, ECS::ComponentMask newMask)
			{
				MANI_ASSERT(oldMask.count() != newMask.count(), "Attempting to get archetypes with the same component count");
				auto findArchetype = [this](const ECS::ComponentMask& mask) -> Archetype*
				{
					if (auto* archetype = m_archetypes.find(mask))
					{
						return archetype->get();
					}
					return nullptr;
				};

				const Mani::Array<ECS::ComponentId, sizeof... (Ts)> componentIds = { this->template getAndRegisterComponentId<Ts>()... };
				Archetype* oldArchetype = findArchetype(oldMask);
				Archetype* newArchetype = findArchetype(newMask);

				if (newArchetype == nullptr && newMask.any())
				{
					constexpr SizeT capacity = INITIAL_COMPONENT_CAPACITY;
					if (oldArchetype == nullptr)
					{
						newArchetype = m_archetypes.add(newMask, std::make_unique<Archetype>(capacity)).get();
					}
					else
					{
						newArchetype = m_archetypes.add(newMask, oldArchetype->makeNew(capacity)).get();
					}

					[&]<SizeT... Is>(std::index_sequence<Is...>)
					{
						if (newMask.count() > oldMask.count())
						{
							 newArchetype->addComponentPools<Ts...>(componentIds[Is]...);
						}
						else
						{
							newArchetype->removeComponentPools<Ts...>(componentIds[Is]...);
						}
					}(std::index_sequence_for<Ts...>{});
				}

				// make sure we get the old archetype after the new archetype in case a new archetype was added.
				MANI_ASSERT(!oldMask.any() || oldArchetype != nullptr, "did not manage to get the old archetype");
				MANI_ASSERT(!newMask.any() || newArchetype != nullptr, "did not manage to create the new archetype");
				return { oldArchetype, newArchetype};
			}

			void lock() const
			{
				m_locks++;
			}

			void unlock() const
			{
				MANI_ASSERT(m_locks > 0, "unlock before lock.");
				m_locks--;
			}

			template<typename T>
			[[nodiscard]] ComponentPool<T>& getOrAddPinnedComponentPool()
			{
				const ECS::ComponentId componentId = getComponentId<T>();
				if (m_pinned[componentId] == nullptr)
				{
					m_pinned[componentId] = std::make_unique<ComponentPool<T>>();
				}
				return *(static_cast<ComponentPool<T>*>(m_pinned[componentId].get()));
			}

			template<typename T>
			[[nodiscard]] ComponentPool<T>* getPinnedComponentPoolPtr()
			{
				const ECS::ComponentId componentId = getComponentId<T>();
				if (!m_pinned.isValid(componentId))
				{
					return nullptr;
				}
				return static_cast<ComponentPool<T>*>(m_pinned[componentId].get());
			}

			template<typename T>
			[[nodiscard]] const ComponentPool<T>* getPinnedComponentPoolPtr() const
			{
				const ECS::ComponentId componentId = getComponentId<T>();
				if (!m_pinned.isValid(componentId))
				{
					return nullptr;
				}
				return static_cast<ComponentPool<T>*>(m_pinned[componentId].get());
			}

			// Entities
			ECS::EntityId m_singletonId = ECS::INVALID_ID;
			Mani::List<ECS::Entity> m_entities;
			Mani::List<ECS::Index> m_recyclableIndices;

			Mani::List<ECS::EntityId> m_markedForDestroy;
			mutable std::mutex m_markedForDestroyMutex;

			// Components
			Mani::Map<ECS::ComponentMask, std::unique_ptr<Archetype>> m_archetypes;
			Mani::Array<std::unique_ptr<IComponentPool>, ECS::MAX_COMPONENTS> m_pinned;

			// Type magement
			Mani::Map<ECS::ComponentId, Dtor> TYPE_DESTRUCTORS;
			Mani::Map<ECS::ComponentId, SizeT> TYPE_SIZES;

			inline static ComponentId TYPE_ID_SEQUENCE = 0;
			template<typename T>
			inline static ComponentId TYPE_ID = TYPE_ID_SEQUENCE++;

			// Thread management
			Mani::ThreadId m_threadId;

			// internals
			mutable std::atomic<SizeT> m_locks;

			inline static constexpr std::string_view INVALID_ENTITY_MESSAGE = "Entity {} is invalid";
			inline static constexpr std::string_view COMPONENT_NOT_FOUND_MESSAGE = "Entity does not have component {}";

			void ASSERT_SAME_THREAD() const
			{
				MANI_ASSERT(m_threadId == Mani::thisThreadId(), "Only allowed on the registry's thread. In most cases it is also the main thread");
			}

			void ASSERT_UNLOCKED() const
			{
				MANI_ASSERT(m_locks == 0, "Operation forbidden while iterating a view, please use a Command Buffer");
			}
		};
	}

	// exposed outside for convenience
	template<typename T>
	using Ref = ECS::ComponentRef<T>;
	template<typename T>
	using LazyRef = ECS::LazyComponentRef<T, ECS::Registry>;
}