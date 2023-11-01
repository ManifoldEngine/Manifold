#pragma once

#include "System.h"
#include <ECS/EntityRegistry.h>
#include <Core/Interfaces/ITickable.h>
#include <vector>
#include <memory>

namespace ECSEngine
{
	// todo: this should probably go into a utils module.
	template<class T, class U>
	concept Derived = std::is_base_of<U, T>::value;

	class SystemContainer : public ITickable
	{
	public:
		virtual void initialize();
		virtual void deinitialize();

		virtual void tick(float deltaTime);

		template<Derived<SystemBase> TSystem>
		bool createSystem();

		template<Derived<SystemBase> TSystem>
		std::shared_ptr<TSystem> getSystem() const;

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
		for (const auto& pSystem : m_systems)
		{
			if (std::dynamic_pointer_cast<const TSystem>(pSystem) != nullptr)
			{
				return false;
			}
		}

		auto pSystem = std::make_shared<TSystem>();
		if (m_bIsInitialized)
		{
			pSystem->initialize(m_registry, *this);
		}
		m_systems.push_back(pSystem);

		return true;
	}

	template<Derived<SystemBase> TSystem>
	inline std::shared_ptr<TSystem> SystemContainer::getSystem() const
	{
		for (auto& pSystem : m_systems)
		{
			if (std::dynamic_pointer_cast<TSystem>(pSystem) != nullptr)
			{
				return std::static_pointer_cast<TSystem>(pSystem);
			}
		}
		return nullptr;
	}

	template<Derived<SystemBase> TSystem>
	inline bool SystemContainer::destroySystem()
	{
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			std::shared_ptr<SystemBase> pSystem = *it;
			if (std::dynamic_pointer_cast<TSystem>(pSystem) != nullptr)
			{
				if (m_bIsInitialized)
				{
					pSystem->deinitialize(m_registry);
				}

				pSystem.reset();
				m_systems.erase(it);
				return true;
			}
		}
		return false;
	}
}