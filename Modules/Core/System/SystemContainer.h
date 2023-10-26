#pragma once

#include "System.h"
#include <ECS/EntityRegistry.h>
#include <Core/Interfaces/ITickable.h>
#include <vector>

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
		TSystem* getSystem() const;

		template<Derived<SystemBase> TSystem>
		bool destroySystem();

	private:
		EntityRegistry m_registry;
		std::vector<SystemBase*> m_systems;
		bool m_bIsInitialized = false;
	};

	template<Derived<SystemBase> TSystem>
	inline bool SystemContainer::createSystem()
	{
		// check if a system of this type exists already.
		for (const auto* pSystem : m_systems)
		{
			if (dynamic_cast<const TSystem*>(pSystem) != nullptr)
			{
				return false;
			}
		}

		m_systems.push_back(new TSystem());

		if (m_bIsInitialized)
		{
			SystemBase* system = m_systems.back();
			system->initialize(m_registry, *this);
		}

		return true;
	}

	template<Derived<SystemBase> TSystem>
	inline TSystem* SystemContainer::getSystem() const
	{
		for (auto* pSystem : m_systems)
		{
			if (dynamic_cast<const TSystem*>(pSystem) != nullptr)
			{
				return static_cast<TSystem*>(pSystem);
			}
		}
		return nullptr;
	}

	template<Derived<SystemBase> TSystem>
	inline bool SystemContainer::destroySystem()
	{
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			SystemBase* pSystem = *it;
			if (dynamic_cast<const TSystem*>(pSystem) != nullptr)
			{
				if (m_bIsInitialized)
				{
					pSystem->deinitialize(m_registry);
				}

				delete pSystem;
				m_systems.erase(it);
				return true;
			}
		}
		return false;
	}
}