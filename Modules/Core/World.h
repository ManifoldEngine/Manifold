#pragma once

#include "Core.h"
#include "Interfaces/ITickable.h"
#include <ECS/EntityRegistry.h>

namespace ECSEngine
{
	class SystemContainer;

	class Core_API World : public ITickable
	{
	public:
		World();
		~World();

		virtual void tick(float deltaTime) override;
		SystemContainer& getSystemContainer();

	private:
		SystemContainer* m_systemContainer = nullptr;
	};
}