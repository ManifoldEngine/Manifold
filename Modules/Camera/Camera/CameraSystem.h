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

		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

		virtual void tick(float deltaTime, ECS::Registry& registry) override;
	};
}