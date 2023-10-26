#pragma once

#include "Core.h"
#include <memory>

namespace ECSEngine
{
	class World;
	
	class Core_API IWorldObject
	{
	public:
		virtual World* getWorld() const = 0;
	};
}