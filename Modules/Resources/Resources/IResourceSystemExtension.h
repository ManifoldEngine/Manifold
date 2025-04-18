#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class IResourceSystemExtension
	{
	public:
		virtual void onResourceCreated(ECS::Registry& registry, ECS::EntityId entityId) const = 0;
		virtual void onResourceDestroyed(ECS::Registry& registry, ECS::EntityId entityId) const = 0;
	};

	struct ResourceSystemExtension
	{
		std::shared_ptr<IResourceSystemExtension> obj;
	};
}