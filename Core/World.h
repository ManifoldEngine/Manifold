#pragma once

#include "Core.h"
#include "Interfaces/ITickable.h"
#include <ECS/EntityRegistry.h>

namespace ECSEngine
{
	class Core_API World : public ITickable
	{
	public:
		virtual void tick(float deltaTime) override;

	private:
		EntityRegistry m_pRegistry;
	};
}