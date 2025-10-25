#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class IResourceSystemExtension
	{
	public:
		// Always called on the main thread through Mani::defer
		virtual void onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const = 0;
		// Always called on the main thread through Mani::defer
		virtual void onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const = 0;
	};
}