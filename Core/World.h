#pragma once

#include "Core.h"
#include "Interfaces/ITickable.h"
#include <ECS/EntityRegistry.h>
#include <vector>
#include <System.h>

namespace ECSEngine
{
	// todo: this should probably go into a utils module.
	template<class T, class U>
	concept Derived = std::is_base_of<U, T>::value;

	class Core_API World : public ITickable
	{
	public:
		virtual void tick(float deltaTime) override;
		
		template<Derived<SystemBase> TSystem>
		bool createSystem();

		template<Derived<SystemBase> TSystem>
		TSystem* getSystem();

		template<Derived<SystemBase> TSystem>
		bool destroySystem();

	private:
		EntityRegistry m_registry;
		// todo: needs pimpl
		std::vector<SystemBase*> m_systems;
	};

	template<Derived<SystemBase> TSystem>
	inline bool World::createSystem()
	{
		for (const auto* pSystem : m_systems)
		{
			if (dynamic_cast<const TSystem*>(pSystem) != nullptr)
			{
				return false;
			}
		}

		m_systems.push_back(new TSystem());
		SystemBase* system = m_systems.back();
		system->initialize(this, m_registry);
		return true;
	}

	template<Derived<SystemBase> TSystem>
	inline TSystem* World::getSystem()
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
	inline bool World::destroySystem()
	{
		for (auto it = m_systems.begin(); it != m_systems.end(); it++)
		{
			SystemBase* pSystem = *it;
			if (dynamic_cast<const TSystem*>(pSystem) != nullptr)
			{
				pSystem->deinitialize(m_registry);
				delete pSystem;
				m_systems.erase(it);
				return true;
			}
		}
		return false;
	}
}