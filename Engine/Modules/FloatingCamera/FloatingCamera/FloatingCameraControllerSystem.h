#pragma once

#include <Core/System/System.h>
#include <memory>
#include <string>

namespace Mani
{
	class EntityRegistry;
	class SystemContainer;
	class InputSystem;

	class FloatingCameraControllerSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry & registry) const override;

		void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;

	private:
		const float CAMERA_SPEED = 10.f;
		float m_previousCameraX = 0.f;
		float m_previousCameraY = 0.f;

		float m_sensitivity = 0.1f;

		std::weak_ptr<InputSystem> m_inputSystem;

		const std::string MOVE_ACTION = "FloatingCameraMove";
		const std::string AIM_ACTION = "FloatingCameraAim";
	};
}
