#include "FloatingCameraSystem.h"

#include <FloatingCamera/FloatingCamera.h>
#include <Camera/CameraSystem.h>
#include <Inputs/Data/InputUser.h>

using namespace Mani;

void FloatingCameraSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
	ECS::EntityId entityId = registry.create();
	registry.add<FloatingCamera>(entityId);
	
	InputUser& inputUser = *registry.add<InputUser>(entityId);
	InputUtils::setAction(inputUser, MOVE_ACTION);
	InputUtils::setAction(inputUser, AIM_ACTION);
	InputUtils::addBinding(inputUser, "WASD", MOVE_ACTION);
	InputUtils::addBinding(inputUser, "Mouse", AIM_ACTION);
}

void FloatingCameraSystem::tick(float deltaTime, ECS::Registry& registry)
{
	ECS::View<FloatingCamera, InputUser> floatingCameraView(registry);
	for (const ECS::EntityId entityId : floatingCameraView)
	{
		InputUser* inputUser = registry.get<InputUser>(entityId);
		FloatingCamera* floatingCamera = registry.get<FloatingCamera>(entityId);

		const InputAction& moveAction = inputUser->actions[MOVE_ACTION];
		const InputAction& aimAction = inputUser->actions[AIM_ACTION];

		ECS::View<Transform, CameraComponent> cameraView(registry);
		if (cameraView.begin() == cameraView.end())
		{
			MANI_LOG_ERROR(LogFloatingCamera, "Could not find a camera in the world");
			continue;
		}

		const ECS::EntityId cameraId = *cameraView.begin();

		Transform* transform = registry.get<Transform>(cameraId);
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
