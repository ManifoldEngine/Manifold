#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	struct FloatingCamera
	{
		float cameraSpeed = 10.f;
		float previousCameraX = 0.f;
		float previousCameraY = 0.f;
		float sensitivity = 0.1f;
	};

	class FloatingCameraSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override { return "FloatingCameraSystem"; }
		virtual bool shouldTick(EntityRegistry& registry) const override { return true; }

		void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;

	private:
		const std::string MOVE_ACTION = "FloatingCameraMove";
		const std::string AIM_ACTION = "FloatingCameraAim";
	};
}
