#include "FloatingCameraSystem.h"

#include <Core/TimeSystem.h>
#include <FloatingCamera/FloatingCamera.h>
#include <Camera/CameraSystem.h>
#include <Inputs/Inputs.h>
#include <ManiMaths/Fwd.h>

using namespace Mani;

const std::string MOVE_ACTION = "FloatingCameraMove";
const std::string AIM_ACTION = "FloatingCameraAim";

void FloatingCameraSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();

	ECS::EntityId entityId = registry.create();
	registry.add<FloatingCamera>(entityId);
	
	registry.add<InputUser>(entityId);
	Inputs::addAction(registry, entityId, MOVE_ACTION);
	Inputs::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Forward,		EInputHints::Keyboard_W);
	Inputs::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Left,			EInputHints::Keyboard_A);
	Inputs::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Back,			EInputHints::Keyboard_S);
	Inputs::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Right,			EInputHints::Keyboard_D);
	Inputs::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Up,				EInputHints::Keyboard_E);
	Inputs::bindActionAxis(registry, entityId, MOVE_ACTION, EInputAxis::Down,			EInputHints::Keyboard_Q);
	Inputs::addAction(registry, entityId, AIM_ACTION, EInputHints::Mouse_Axis);

	// assign all devices to this input user by default.
	for (const auto [deviceId, _] : ECS::ConstView<InputDevice>(registry))
	{
		Inputs::assignDevice(registry, entityId, deviceId);
	}
}

void FloatingCameraSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	for (const auto [entityId, _] : ECS::View<FloatingCamera>(registry))
	{
		registry.deferDestroy(entityId);
	}
}

void FloatingCameraSystem::tick(ECS::Registry& registry)
{
	Ref<Time> time = registry.getSingle<Time>();
	ECS::View<FloatingCamera, InputUser> view(registry);
	for (auto [entityId, floatingCamera, inputUser] : view)
	{
		const InputAction& moveAction = Inputs::getAction(registry, entityId, MOVE_ACTION);
		const InputAction& aimAction = Inputs::getAction(registry, entityId, AIM_ACTION);

		ECS::EntityId cameraId = Cameras::getMainCameraId(registry);
		if (cameraId == ECS::INVALID_ID)
		{
			MANI_LOG_ERROR(LogFloatingCamera, "Could not find a camera in the world");
			continue;
		}

		auto position = registry.get<Position>(cameraId);
		auto rotation = registry.get<Rotation>(cameraId);
		auto camera = registry.get<Camera>(cameraId);
		
		*position +=   (Transform::right(*rotation) * static_cast<float>(moveAction.x) +
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
		*rotation = quatYaw * (*rotation) * quatPitch;

		floatingCamera.previousCameraX = aimX;
		floatingCamera.previousCameraY = aimY;
	}
}
