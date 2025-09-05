#include "FloatingCameraSystem.h"

#include <Core/TimeSystem.h>
#include <FloatingCamera/FloatingCamera.h>
#include <Camera/CameraSystem.h>
#include <Inputs/Data/InputUser.h>
#include <Inputs/Data/InputDevice.h>
#include <ManiMaths/Fwd.h>

using namespace Mani;

void FloatingCameraSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();

	ECS::EntityId entityId = registry.create();
	registry.add<FloatingCamera>(entityId);
	
	InputUser& inputUser = *registry.add<InputUser>(entityId);
	inputUser.setAction(MOVE_ACTION);
	inputUser.setAction(AIM_ACTION);
	inputUser.addBinding("WASD", MOVE_ACTION);
	inputUser.addBinding("Mouse", AIM_ACTION);

	// assign all devices to this input user by default.
	for (const auto entityId : ECS::View<InputDevice>(registry))
	{
		inputUser.inputDevices.push_back(entityId);
	}
}

void FloatingCameraSystem::tick(ECS::Registry& registry)
{
	ECS::View<FloatingCamera, InputUser> floatingCameraView(registry);
	for (const ECS::EntityId entityId : floatingCameraView)
	{
		InputUser& inputUser = *registry.get<InputUser>(entityId);
		FloatingCamera& floatingCamera = *registry.get<FloatingCamera>(entityId);

		const InputAction& moveAction = inputUser.actions[MOVE_ACTION];
		const InputAction& aimAction = inputUser.actions[AIM_ACTION];

		ECS::View<Position, Rotation, Camera> cameraView(registry);
		if (cameraView.begin() == cameraView.end())
		{
			MANI_LOG_ERROR(LogFloatingCamera, "Could not find a camera in the world");
			continue;
		}

		const ECS::EntityId cameraId = *cameraView.begin();
		Position& position = *registry.get<Position>(cameraId);
		Rotation& rotation = *registry.get<Rotation>(cameraId);

		Time& time = *registry.getSingle<Time>();
		position.value +=  (Transform::right(rotation) * static_cast<float>(moveAction.x) +
							Transform::up(rotation) * static_cast<float>(moveAction.y) +
							Transform::forward(rotation) * static_cast<float>(moveAction.z)) *
							time.delta * floatingCamera.cameraSpeed;

		const float aimX = static_cast<float>(aimAction.x);
		const float aimY = static_cast<float>(aimAction.y);

		const float deltaX = floatingCamera.previousCameraX - aimX;
		const float deltaY = floatingCamera.previousCameraY - aimY;

		if (Math::abs(deltaX) <= FLT_EPSILON || Math::abs(deltaX) <= FLT_EPSILON)
		{
			continue;
		}

		const float yaw = deltaX * floatingCamera.sensitivity;
		const float pitch = deltaY * floatingCamera.sensitivity;

		Quatf quatPitch = Quatf::axisAngleDeg(-pitch, VEC3F::RIGHT);
		Quatf quatYaw = Quatf::axisAngleDeg(yaw, VEC3F::UP);

		// it is crucial to respect this order of operation to avoid unintended roll.
		rotation.value = quatYaw * rotation.value * quatPitch;

		floatingCamera.previousCameraX = aimX;
		floatingCamera.previousCameraY = aimY;
	}
}
