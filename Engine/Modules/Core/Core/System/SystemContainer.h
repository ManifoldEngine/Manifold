#pragma once

#include "System.h"
#include <Core/ManiAssert.h>
#include <ECS/EntityRegistry.h>
#include <Utils/TemplateUtils.h>
#include <vector>
#include <memory>

namespace Mani
{
	// System container class. It manages unique systems. It also owns an EntityRegistry and is in charge
	// of distributing the registry to systems.
	class SystemContainer
	{
	public:
		// Initializes all created systems. once this is called, will initialize newly created systems
		void initialize();

		// Deinitiailize all systems
		void deinitialize();

		void tick(float deltaTime);

		// creates a new TSystem : public SystemBase
		// if the container is initialized, the system will be initialized as well
		// if a system of type TSystem already exists, a new system will not be created.
		// after this is called, a TSystem is guarranteed to live in the container.
		// returns true if a system was created.
		template<Derived<SystemBase> TSystem>
		SystemContainer& createSystem();
		
		template<typename TSystem>
		std::weak_ptr<TSystem> getSystem();

		// creates, initializes then return a shared pointer to the TSystem
		// this is most notably useful to allow a system to initialize a dependency and receive a pointer to it
		// returns a weak pointer to a TSystem
		template<Derived<SystemBase> TSystem>
		std::weak_ptr<TSystem> initializeDependency();

		// destroys a system of type TSystem.
		// after this is called, no TSystem remains in the container.
		// returns true if a system was destroyed
		template<Derived<SystemBase> TSystem>
		SystemContainer& destroySystem();

		// returns the amount of systems
		size_t size() const;

	private:
		EntityRegistry m_registry;
		std::vector<std::shared_ptr<SystemBase>> m_systems;
		bool m_isInitialized = false;
	};

	template<Derived<SystemBase> TSystem>
	inline SystemContainer& SystemContainer::createSystem()
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
		if (m_isInitialized)
		{
			system->initialize(m_registry, *this);
		}

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

		m_systems.insert(insertIt, system);
		return *this;
	}

	template<typename TSystem>
	inline std::weak_ptr<TSystem> SystemContainer::getSystem()
	{
		for (auto& system : m_systems)
		{
			if (std::dynamic_pointer_cast<TSystem>(system) != nullptr)
			{
				return std::dynamic_pointer_cast<TSystem>(system);
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
	inline SystemContainer& SystemContainer::destroySystem()
	{
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			std::shared_ptr<SystemBase> system = *it;
			if (std::dynamic_pointer_cast<TSystem>(system) != nullptr)
			{
				if (m_isInitialized)
				{
					system->deinitialize(m_registry);
				}

				system.reset();
				m_systems.erase(it);
				return *this;
			}
		}
		return *this;
	}

	inline void SystemContainer::initialize()
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

	inline void SystemContainer::deinitialize()
	{
		if (!m_isInitialized)
		{
			return;
		}

		for (auto it = m_systems.rbegin(); it != m_systems.rend(); it++)
		{
			(*it)->deinitialize(m_registry);
		}

		m_isInitialized = false;
	}

	inline void SystemContainer::tick(float deltaTime)
	{
		if (!m_isInitialized)
		{
			return;
		}

		for (auto& system : m_systems)
		{
			if (system->shouldTick(m_registry) && system->isEnabled())
			{
				system->tick(deltaTime, m_registry);
			}
		}
	}

	inline size_t SystemContainer::size() const
	{
		return m_systems.size();
	}
}