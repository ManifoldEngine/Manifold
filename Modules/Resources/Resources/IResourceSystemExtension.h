#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class IResourceSystemExtension
	{
	public:
		virtual void onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId) const = 0;
		virtual void onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId) const = 0;
	};

	struct ResourceSystemExtension
	{
		std::shared_ptr<IResourceSystemExtension> obj;
	};
}