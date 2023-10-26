#pragma once

#include "Core.h"

namespace ECSEngine
{
	class Core_API ITickable
	{
	public:
		virtual void tick(float deltaTime) = 0;
	};
}