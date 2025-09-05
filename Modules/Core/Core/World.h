#pragma once

#include "System.h"

#include <Core/ManiAssert.h>
#include <Core/ManiTraits.h>

#include <Core/ECS/Registry.h>
#include <Core/ECS/System.h>

#include <vector>
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
		template<IsDerived<ECS::System> TSystem>
		World& createSystem();
		
		// creates, initializes then return a shared pointer to the TSystem
		// this is most notably useful to allow a system to initialize a dependency and receive a pointer to it
		// returns a weak pointer to a TSystem
		template<IsDerived<ECS::System> TSystem>
		void initializeDependency();

		// returns true if TSystem was created in this world.
		template<typename TSystem>
		bool has() const;

		// destroys a system of type TSystem.
		// after this is called, no TSystem remains in the container.
		// returns true if a system was destroyed
		template<IsDerived<ECS::System> TSystem>
		World& destroySystem();

		// returns the amount of systems
		size_t systemCount() const;

		const ECS::Registry& getRegistry() const { return m_registry; }
		ECS::Registry& getMutableRegistry() { return m_registry; }

	private:
		ECS::Registry m_registry;
		std::vector<std::shared_ptr<ECS::System>> m_systems;
		bool m_isInitialized = false;
	};

	template<IsDerived<ECS::System> TSystem>
	inline World& World::createSystem()
	{
		// check if a system of this type exists already.
		for (const auto& system : m_systems)
		{
			if (std::dynamic_pointer_cast<const TSystem>(system) != nullptr)
			{
				return *this;
			}
		}

		auto system = std::make_shared<TSystem>();

		ETickGroup targetTickGroup = system->getTickGroup();
		auto insertIt = m_systems.end();
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			if ((*it)->getTickGroup() > targetTickGroup)
			{
				insertIt = it;
				break;
			}
		}

		m_systems.emplace(insertIt, system);

		if (m_isInitialized)
		{
			system->initialize(m_registry, *this);
		}
		return *this;
	}

	template<IsDerived<ECS::System> TSystem>
	inline void World::initializeDependency()
	{
		createSystem<TSystem>();
		for (auto& system : m_systems)
		{
			if (isDerived<TSystem>(system))
			{
				system->initialize(m_registry, *this);
			}
		}
	}

	template<typename TSystem>
	inline bool World::has() const
	{
		for (auto& system : m_systems)
		{
			if (isDerived<TSystem>(*system))
			{
				return true;
			}
		}
		return false;
	}

	template<IsDerived<ECS::System> TSystem>
	inline World& World::destroySystem()
	{
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			std::shared_ptr<ECS::System> system = *it;
			if (std::dynamic_pointer_cast<TSystem>(system) != nullptr)
			{
				if (m_isInitialized)
				{
					system->deinitialize(m_registry, *this);
				}
				return *this;
			}
		}
		return *this;
	}

	inline void World::initialize()
	{
		if (m_isInitialized)
		{
			return;
		}

		for (auto& system : m_systems)
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

		for (auto it = m_systems.rbegin(); it != m_systems.rend(); it++)
		{
			(*it)->deinitialize(m_registry, *this);
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
			std::vector<std::shared_ptr<ECS::System>> systems = m_systems;
			for (auto& system : systems)
			{
				if (system->shouldTick(m_registry))
				{
					system->tick(m_registry);
				}
			}
		}

		m_registry.handleDeferredDestroy();

		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			if (!(*it)->isInitialized())
			{
				m_systems.erase(it);
			}
		}
	}

	inline size_t World::systemCount() const
	{
		return m_systems.size();
	}
}