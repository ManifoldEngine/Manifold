#pragma once

#include <Core/System/System.h>
#include <memory>

namespace ECSEngine
{
	class EntityRegistry;
	class SystemContainer;
	class InputSystem;
}

class DebugCameraControllerSystem : public ECSEngine::SystemBase
{
public:
	virtual std::string_view getName() const override;
	virtual bool shouldTick(ECSEngine::EntityRegistry & registry) const override;

	void onInitialize(ECSEngine::EntityRegistry& registry, ECSEngine::SystemContainer& systemContainer) override;

	virtual void tick(float deltaTime, ECSEngine::EntityRegistry& registry);

private:
	const float CAMERA_SPEED = 5.f;
	float m_previousCameraX = 0.f;
	float m_previousCameraY = 0.f;

	float m_yaw = 0.f;
	float m_pitch = 0.f;

	std::weak_ptr<ECSEngine::InputSystem> m_inputSystem;

	const std::string MOVE_ACTION = "DebugCameraMove";
	const std::string AIM_ACTION = "DebugCameraAim";
};