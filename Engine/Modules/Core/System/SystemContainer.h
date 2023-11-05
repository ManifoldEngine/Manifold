#pragma once

#include "System.h"
#include <ECS/EntityRegistry.h>
#include <Core/Interfaces/ITickable.h>
#include <Utils/TemplateUtils.h>
#include <vector>
#include <memory>

namespace ECSEngine
{
	// System container class. It manages unique systems. It also owns an EntityRegistry and is in charge
	// of distributing the registry to systems.
	class SystemContainer : public ITickable
	{
	public:
		// Initializes all created systems. once this is called, will initialize newly created systems
		virtual void initialize();
		// Deinitiailize all systems
		virtual void deinitialize();

		virtual void tick(float deltaTime);

		// creates a new TSystem : public SystemBase
		// if the container is initialized, the system will be initialized as well
		// if a system of type TSystem already exists, a new system will not be created.
		// after this is called, a TSystem is guarranteed to live in the container.
		// returns true if a system was created.
		template<Derived<SystemBase> TSystem>
		bool createSystem();

		template<Derived<SystemBase> TSystem>
		std::weak_ptr<TSystem> getSystem() const;

		// creates, initializes then return a shared pointer to the TSystem
		// this is most notably useful to allow a system to initialize a dependency and receive a pointer to it
		// returns a weak pointer to a TSystem
		template<Derived<SystemBase> TSystem>
		std::weak_ptr<TSystem> initializeDependency();

		// destroys a system of type TSystem.
		// after this is called, no TSystem remains in the container.
		// returns true if a system was destroyed
		template<Derived<SystemBase> TSystem>
		bool destroySystem();

	private:
		EntityRegistry m_registry;
		std::vector<std::shared_ptr<SystemBase>> m_systems;
		bool m_bIsInitialized = false;
	};

	template<Derived<SystemBase> TSystem>
	inline bool SystemContainer::createSystem()
	{
		// check if a system of this type exists already.
		for (const auto& system : m_systems)
		{
			if (std::dynamic_pointer_cast<const TSystem>(system) != nullptr)
			{
				return false;
			}
		}

		auto system = std::make_shared<TSystem>();
		if (m_bIsInitialized)
		{
			system->initialize(m_registry, *this);
		}
		m_systems.push_back(system);

		return true;
	}

	template<Derived<SystemBase> TSystem>
	inline std::weak_ptr<TSystem> SystemContainer::getSystem() const
	{
		for (auto& system : m_systems)
		{
			if (std::dynamic_pointer_cast<TSystem>(system) != nullptr)
			{
				return std::static_pointer_cast<TSystem>(system);
			}
		}
		return std::weak_ptr<TSystem>();
	}

	template<Derived<SystemBase> TSystem>
	inline std::weak_ptr<TSystem> SystemContainer::initializeDependency()
	{
		createSystem<TSystem>();
		std::weak_ptr<TSystem> system = getSystem<TSystem>();
		if (!system.expired())
		{
			system.lock()->initialize(m_registry, *this);
		}
		return system;
	}

	template<Derived<SystemBase> TSystem>
	inline bool SystemContainer::destroySystem()
	{
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			std::shared_ptr<SystemBase> system = *it;
			if (std::dynamic_pointer_cast<TSystem>(system) != nullptr)
			{
				if (m_bIsInitialized)
				{
					system->deinitialize(m_registry);
				}

				system.reset();
				m_systems.erase(it);
				return true;
			}
		}
		return false;
	}
}