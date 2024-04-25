#include "FloatingCameraSystem.h"

#include <FloatingCamera/FloatingCamera.h>
#include <Camera/CameraSystem.h>
#include <Inputs/Data/InputUser.h>

using namespace Mani;

void FloatingCameraSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	EntityId entityId = registry.create();
	registry.addComponent<FloatingCamera>(entityId);
	InputUser* inputUser = registry.addComponent<InputUser>(entityId);
	inputUser->actions =
	{
		{ MOVE_ACTION, { MOVE_ACTION }},
		{ AIM_ACTION, { AIM_ACTION }}
	};

	inputUser->bindings =
	{
		{ "WASD", { MOVE_ACTION } },
		{ "Mouse", { AIM_ACTION } },
	};
}

void FloatingCameraSystem::tick(float deltaTime, EntityRegistry& registry)
{
	RegistryView<FloatingCamera, InputUser> floatingCameraView(registry);
	for (const EntityId entityId : floatingCameraView)
	{
		InputUser* inputUser = registry.getComponent<InputUser>(entityId);
		FloatingCamera* floatingCamera = registry.getComponent<FloatingCamera>(entityId);

		const InputAction& moveAction = inputUser->actions[MOVE_ACTION];
		const InputAction& aimAction = inputUser->actions[AIM_ACTION];

		RegistryView<Transform, CameraComponent> cameraView(registry);
		if (cameraView.begin() == cameraView.end())
		{
			MANI_LOG_ERROR(LogFloatingCamera, "Could not find a camera in the world");
			continue;
		}

		const EntityId cameraId = *cameraView.begin();

		Transform* transform = registry.getComponent<Transform>(cameraId);
		if (transform == nullptr)
		{
			continue;
		}

		transform->position += (transform->right() * static_cast<float>(moveAction.x) +
			transform->up() * static_cast<float>(moveAction.y) +
			transform->forward() * static_cast<float>(moveAction.z)) *
			deltaTime * floatingCamera->cameraSpeed;

		const float aimX = static_cast<float>(aimAction.x);
		const float aimY = static_cast<float>(aimAction.y);

		const float deltaX = floatingCamera->previousCameraX - aimX;
		const float deltaY = floatingCamera->previousCameraY - aimY;

		if (std::abs(deltaX) <= FLT_EPSILON || std::abs(deltaX) <= FLT_EPSILON)
		{
			continue;
		}

		const float yaw = deltaX * floatingCamera->sensitivity;
		const float pitch = deltaY * floatingCamera->sensitivity;

		glm::quat quatPitch = glm::angleAxis(glm::radians(-pitch), glm::vec3(1.f, 0.f, 0.f));
		glm::quat quatYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));

		// it is crucial to respect this order of operation to avoid unintended roll.
		transform->rotation = quatYaw * transform->rotation * quatPitch;

		floatingCamera->previousCameraX = aimX;
		floatingCamera->previousCameraY = aimY;
	}
}
