#pragma once

#include "System.h"

#include <Core/ManiAssert.h>
#include <Core/ManiTraits.h>

#include <Core/Containers/List.h>

#include <Core/ECS/Registry.h>

#include <memory>

namespace Mani
{
	// Manages unique systems. It also owns a Registry and is in charge of distributing the registry to systems.
	class World
	{
	public:
		// Initializes all created systems. once this is called, will initialize newly created systems
		void initialize();

		// Deinitiailize all systems
		void deinitialize();

		void tick();

		// creates a new TSystem : public ECS::System
		// if the container is initialized, the system will be initialized as well
		// if a system of type TSystem already exists, a new system will not be created.
		// after this is called, a TSystem is guarranteed to live in the container.
		// returns true if a system was created.
		template<DerivedFrom<ECS::System> TSystem>
		World& createSystem();
	
		template<typename... TSystems>
		requires (DerivedFrom<TSystems, ECS::System> && ...)
		World& createSystems();

		template<typename... Ts>
		World& createSystems(TypeList<Ts...>&&);
		
		// creates, initializes then return a shared pointer to the TSystem
		// this is most notably useful to allow a system to initialize a dependency and receive a pointer to it
		// returns a weak pointer to a TSystem
		template<DerivedFrom<ECS::System> TSystem>
		void initializeDependency();

		// returns true if TSystem was created in this world.
		template<typename TSystem>
		bool has() const;

		template<typename... TSystems>
		requires (DerivedFrom<TSystems, ECS::System> && ...)
		bool hasSystems() const;
		
		template <typename... Ts>
		bool hasSystems(TypeList<Ts...>&&) const;

		// destroys a system of type TSystem.
		// after this is called, no TSystem remains in the container.
		// returns true if a system was destroyed
		template<DerivedFrom<ECS::System> TSystem>
		World& destroySystem();

		template<DerivedFrom<ECS::System>... TSystems>
		requires (DerivedFrom<TSystems, ECS::System> && ...)
		World& destroySystems();

		template<typename... Ts>
		World& destroySystems(TypeList<Ts...>&&);

		// returns the amount of systems
		size_t systemCount() const;

		bool isInitialized() const { return m_isInitialized; }

		const ECS::Registry& getRegistry() const { return m_registry; }
		ECS::Registry& getMutableRegistry() { return m_registry; }

	private:
		struct SystemContainer
		{
			std::shared_ptr<ECS::System> system = nullptr;
			bool isMarkedForDestruction = false;
		};

		ECS::Registry m_registry;
		List<SystemContainer> m_systems;
		bool m_isInitialized = false;
	};

	template<DerivedFrom<ECS::System> TSystem>
	inline World& World::createSystem()
	{
		// check if a system of this type exists already.
		for (const auto& [system, _]: m_systems)
		{
			if (std::dynamic_pointer_cast<const TSystem>(system) != nullptr)
			{
				return *this;
			}
		}

		auto system = std::make_shared<TSystem>();

		constexpr bool isMarkedForDestruction = false;
		m_systems.add(SystemContainer{ system, isMarkedForDestruction });

		m_systems.sort([](const auto& lhs, const auto& rhs) { return lhs.system->getTickGroup() < rhs.system->getTickGroup(); });

		if (m_isInitialized)
		{
			system->initialize(m_registry, *this);
		}
		return *this;
	}

	template<typename... TSystems>
	requires (DerivedFrom<TSystems, ECS::System> && ...)
	inline World& World::createSystems()
	{
		(createSystem<TSystems>(), ...);
		return *this;
	}

	template<typename ...Ts>
	inline World& World::createSystems(TypeList<Ts...>&&)
	{
		return createSystems<Ts...>();
	}

	template<DerivedFrom<ECS::System> TSystem>
	inline void World::initializeDependency()
	{
		createSystem<TSystem>();
		for (auto& [system, _] : m_systems)
		{
			if (isDerivedFrom<TSystem>(system))
			{
				system->initialize(m_registry, *this);
			}
		}
	}

	template<typename TSystem>
	inline bool World::has() const
	{
		for (auto& [system, isMarkedForDestruction] : m_systems)
		{
			if (!isMarkedForDestruction && isDerivedFrom<TSystem>(*system))
			{
				return true;
			}
		}
		return false;
	}

	template<typename... TSystems>
	requires (DerivedFrom<TSystems, ECS::System> && ...)
	inline bool World::hasSystems() const
	{
		return (has<TSystems>() && ...);
	}

	template<typename ...Ts>
	inline bool World::hasSystems(TypeList<Ts...>&&) const
	{
		return hasSystems<Ts...>();
	}

	template<DerivedFrom<ECS::System> TSystem>
	inline World& World::destroySystem()
	{
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			auto& [system, isMarkedForDestruction] = *it;
			if (std::dynamic_pointer_cast<TSystem>(system) != nullptr)
			{
				if (m_isInitialized)
				{
					system->deinitialize(m_registry, *this);
				}
				isMarkedForDestruction = true;
				return *this;
			}
		}
		return *this;
	}

	template<DerivedFrom<ECS::System>... TSystems>
	requires (DerivedFrom<TSystems, ECS::System> && ...)
	inline World& World::destroySystems()
	{
		(destroySystem<TSystems>(), ...);
		return *this;
	}

	template<typename ...Ts>
	inline World& World::destroySystems(TypeList<Ts...>&&)
	{
		return destroySystems<Ts...>();
	}

	inline void World::initialize()
	{
		if (m_isInitialized)
		{
			return;
		}

		for (auto& [system, _] : m_systems)
		{
			system->initialize(m_registry, *this);
		}

		m_isInitialized = true;
	}

	inline void World::deinitialize()
	{
		if (!m_isInitialized)
		{
			return;
		}

		for (SizeT i = m_systems.count() - 1; i != INDEX_NONE; i--)
		{
			auto& [system, _] = m_systems[i];
			system->deinitialize(m_registry, *this);
		}

		// it is possible we have deferred entities left.
		m_registry.handleDeferredDestroy();

		m_isInitialized = false;
	}

	inline void World::tick()
	{
		if (!m_isInitialized)
		{
			return;
		}

		{
			// snapshot the systems that should tick. 
			// New systems can be created during a tick and they might not be in a proper state to tick yet.
			List<SystemContainer> systems = m_systems;
			for (auto& [system, _] : systems)
			{
				if (system->shouldTick(m_registry))
				{
					system->tick(m_registry);
				}
			}
		}

		m_registry.handleDeferredDestroy();

		// remove uninitialized systems
		m_systems.removeIf([](const auto& container) { return container.isMarkedForDestruction; });
	}

	inline size_t World::systemCount() const
	{
		return m_systems.count();
	}
}