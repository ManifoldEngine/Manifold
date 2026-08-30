#pragma once

#include "System.h"

#include <Core/ManiAssert.h>
#include <Core/ManiTraits.h>

#include <Core/Containers/List.h>

#include <Core/ECS/Registry.h>

#include <memory>
#include <limits>

namespace Mani
{
	using SystemId = unsigned int;
	constexpr SystemId INVALID_SYSTEM_ID = (std::numeric_limits<SystemId>::max)();

	// Manages unique systems. It also owns a Registry and is in charge of distributing the registry to systems.
	class World
	{
	public:
		// Initializes all created systems. once this is called, will initialize newly created systems
		void initialize();

		// Deinitiailize all systems
		void deinitialize();

		void tick();

		template<typename T>
		static SystemId getSystemId() { return SYSTEM_ID<T>; }

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
		template<typename T>
		World& destroySystem();

		template<typename ...Ts>
		World& destroySystems();

		template<typename ...Ts>
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
			SystemId id = INVALID_SYSTEM_ID;
		};

		ECS::Registry m_registry;
		List<SystemContainer> m_systems;
		bool m_isInitialized = false;

		inline static SystemId SYSTEM_ID_SEQUENCE = 0;
		template<typename T>
		inline static SystemId SYSTEM_ID = SYSTEM_ID_SEQUENCE++;
	};

	template<DerivedFrom<ECS::System> TSystem>
	inline World& World::createSystem()
	{
		const SystemId id = World::getSystemId<TSystem>();
		// check if a system of this type exists already.
		for (const auto& container: m_systems)
		{
			if (container.id == id)
			{
				return *this;
			}
		}

		auto system = std::make_shared<TSystem>();

		constexpr bool isMarkedForDestruction = false;
		m_systems.add(SystemContainer{ system, isMarkedForDestruction, id });

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
		for (auto& container : m_systems)
		{
			if (isDerivedFrom<TSystem>(container.system))
			{
				container.system->initialize(m_registry, *this);
			}
		}
	}

	template<typename TSystem>
	inline bool World::has() const
	{
		for (auto& container : m_systems)
		{
			if (!container.isMarkedForDestruction && isDerivedFrom<TSystem>(*container.system))
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

	template<typename T>
	inline World& World::destroySystem()
	{
		const SystemId id = World::getSystemId<T>();
		for (auto& container : m_systems)
		{
			if (container.id == id)
			{
				if (m_isInitialized)
				{
					container.system->deinitialize(m_registry, *this);
				}
				container.isMarkedForDestruction = true;
				return *this;
			}
		}
		return *this;
	}

	template<typename ...Ts>
	inline World& World::destroySystems()
	{
		(destroySystem<Ts>(), ...);
		return *this;
	}

	template<typename ...Ts>
	inline World& World::destroySystems(TypeList<Ts...>&&)
	{
		return destroySystems<Ts...>();
	}
}