#include "FloatingCameraSystem.h"

#include <Core/TimeSystem.h>
#include <FloatingCamera/FloatingCamera.h>
#include <Camera/CameraSystem.h>
#include <Inputs/Inputs.h>
#include <ManiMaths/Fwd.h>

using namespace Mani;

void FloatingCameraSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();

	ECS::EntityId entityId = registry.create();
	registry.add<FloatingCamera>(entityId);
	
	registry.add<InputUser>(entityId);
	InputsStatics::addAction(registry, entityId, MOVE_ACTION);
	InputsStatics::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Up,		EInputHints::Keyboard_W);
	InputsStatics::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Right,	EInputHints::Keyboard_A);
	InputsStatics::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Down,	EInputHints::Keyboard_S);
	InputsStatics::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Left,	EInputHints::Keyboard_D);
	InputsStatics::addAction(registry, entityId, AIM_ACTION, EInputHints::Mouse_Axis);

	// assign all devices to this input user by default.
	for (const auto [deviceId, _] : ECS::ConstView<InputDevice>(registry))
	{
		InputsStatics::assignDevice(registry, entityId, deviceId);
	}
}

void FloatingCameraSystem::tick(ECS::Registry& registry)
{
	Ref<Time> time = registry.getSingle<Time>();
	ECS::View<FloatingCamera, InputUser> view(registry);
	for (auto [entityId, floatingCamera, inputUser] : view)
	{
		const InputAction& moveAction = InputsStatics::getAction(registry, entityId, MOVE_ACTION);
		const InputAction& aimAction = InputsStatics::getAction(registry, entityId, AIM_ACTION);

		ECS::EntityId cameraId = CameraStatics::getMainCameraId(registry);
		if (cameraId != ECS::INVALID_ID)
		{
			MANI_LOG_ERROR(LogFloatingCamera, "Could not find a camera in the world");
			continue;
		}

		auto position = registry.get<Position>(cameraId);
		auto rotation = registry.get<Rotation>(cameraId);
		auto camera = registry.get<Camera>(cameraId);
		
		position->value +=  (Transform::right(*rotation) * static_cast<float>(moveAction.x) +
							Transform::up(*rotation) * static_cast<float>(moveAction.y) +
							Transform::forward(*rotation) * static_cast<float>(moveAction.z)) *
							time->delta * floatingCamera.cameraSpeed;

		const float aimX = static_cast<float>(aimAction.x);
		const float aimY = static_cast<float>(aimAction.y);

		const float deltaX = floatingCamera.previousCameraX - aimX;
		const float deltaY = floatingCamera.previousCameraY - aimY;

		if (Math::abs(deltaX) <= FLT_EPSILON || Math::abs(deltaY) <= FLT_EPSILON)
		{
			continue;
		}

		const float yaw = deltaX * floatingCamera.sensitivity;
		const float pitch = deltaY * floatingCamera.sensitivity;

		Quatf quatPitch = Quatf::axisAngleDeg(pitch, VEC3F::RIGHT);
		Quatf quatYaw = Quatf::axisAngleDeg(yaw, VEC3F::UP);

		// it is crucial to respect this order of operation to avoid unintended roll.
		rotation->value = quatYaw * rotation->value * quatPitch;

		floatingCamera.previousCameraX = aimX;
		floatingCamera.previousCameraY = aimY;
	}
}
