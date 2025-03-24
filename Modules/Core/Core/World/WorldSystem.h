#pragma once

#include <Core/Core.h>
#include <Core/System/System.h>
#include <ECS/Entity.h>

// forwarding World header for convenience
#include <Core/World/World.h>

namespace Mani
{
	class WorldSystem : public SystemBase
	{
	public:

		virtual std::string_view getName() const override { return "WorldSystem"; }
		virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

		static ECS::EntityId createWorld(ECS::Registry& registry);
		static void destroyWorld(ECS::Registry& registry, ECS::EntityId entityId);

		virtual void tick(float deltaTime, ECS::Registry& registry) override;
	};
}