#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class FloatingCameraSystem : public ECS::SystemT<FloatingCameraSystem, ETick::Enabled>
	{
	public:
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PostUpdate; }
		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry, World& world) override;
		virtual void tick(ECS::Registry& registry) override;
	};
}
