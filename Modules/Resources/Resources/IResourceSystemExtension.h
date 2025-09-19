#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class IResourceSystemExtension
	{
	public:
		virtual void onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const = 0;
		virtual void onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const = 0;
	};
}