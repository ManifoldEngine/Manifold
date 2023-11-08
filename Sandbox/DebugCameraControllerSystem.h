#pragma once

#include <Core/System/System.h>

namespace ECSEngine
{
	class EntityRegistry;
}

class DebugCameraControllerSystem : public ECSEngine::SystemBase
{
	virtual std::string_view getName() const override;
	virtual bool shouldTick(ECSEngine::EntityRegistry & registry) const override;

	virtual void tick(float deltaTime, ECSEngine::EntityRegistry& registry);

private:
	const float CAMERA_SPEED = 5.f;
	float yaw = 0.0f;
	float pitch = 0.0f;
};