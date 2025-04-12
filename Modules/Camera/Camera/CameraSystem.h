#pragma once

#include <Core/CoreFwd.h>
// forwarded for convenience
#include <Camera/Camera.h>

namespace Mani
{
	class CameraSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(ECS::Registry& registry) const override;

		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

		virtual void tick(float deltaTime, ECS::Registry& registry) override;

		static Vec2f worldToScreenSpace(const ECS::Registry& registry, const Vec3f& position);
		static Vec3f screenToWorldSpace(const ECS::Registry& registry, const Vec2f& position);
		static Vec3f screenToWorldProjection(const ECS::Registry& regitry, const Vec2f& position, float distance);

	private:
		static const Camera* getCamera(const ECS::Registry& registry);
		static const Transform* getTransform(const ECS::Registry& registry);
	};
}