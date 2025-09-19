#pragma once

#include <Core/CoreFwd.h>
// forwarded for convenience
#include <Camera/Camera.h>

namespace Mani
{
	class CameraSystem : public ECS::System
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(const ECS::Registry& registry) const override;
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PreUpdate; }

		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

		virtual void tick(ECS::Registry& registry) override;

		static Vec2f worldToScreenSpace(const ECS::Registry& registry, const Vec3f& position);
		static Vec3f screenToWorldSpace(const ECS::Registry& registry, const Vec2f& position);
		static Vec3f screenToWorldProjection(const ECS::Registry& regitry, const Vec2f& position, float distance);
	};
}