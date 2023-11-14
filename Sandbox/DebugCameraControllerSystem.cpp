#include "DebugCameraControllerSystem.h"

#include <Camera/CameraSystem.h>
#include <ECS/Entity.h>
#include <ECS/RegistryView.h>
#include <Core/Components/Transform.h>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Log.h>
#include <Inputs/InputSystem.h>

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <glm/gtc/quaternion.hpp>

using namespace ECSEngine;

std::string_view DebugCameraControllerSystem::getName() const
{
	return "DebugCameraControllerSystem";
}

bool DebugCameraControllerSystem::shouldTick(EntityRegistry& registry) const
{
	return true;
}

void DebugCameraControllerSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_inputSystem = systemContainer.initializeDependency<InputSystem>();
	if (!m_inputSystem.expired())
	{
		std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
		inputSystem->addAction(LOCAL_USERID, MOVE_ACTION);
		inputSystem->addAction(LOCAL_USERID, AIM_ACTION);

		inputSystem->addBinding(LOCAL_USERID, MOVE_ACTION, "WASD");
		inputSystem->addBinding(LOCAL_USERID, AIM_ACTION, "Mouse");
	}
}

void DebugCameraControllerSystem::tick(float deltaTime, ECSEngine::EntityRegistry& registry)
{
	if (m_inputSystem.expired())
	{
		return;
	}

	std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();

	const InputAction* moveAction = inputSystem->getAction(0, MOVE_ACTION);
	const InputAction* aimAction = inputSystem->getAction(0, AIM_ACTION);

	if (moveAction == nullptr || aimAction == nullptr)
	{
		return;
	}

	RegistryView<Transform, CameraComponent> view(registry);
	for (const EntityId& entityId : view)
	{
		Transform* transform = registry.getComponent<Transform>(entityId);
		if (transform == nullptr)
		{
			continue;
		}

		transform->position += glm::vec3(moveAction->x, moveAction->y, moveAction->z)  * deltaTime * CAMERA_SPEED;

		const float deltaX = m_previousCameraX - aimAction->x;
		const float deltaY = m_previousCameraY - aimAction->y;

		m_yaw += deltaX;
		m_pitch += deltaY;

		glm::vec3 forward;
		forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		forward.y = sin(glm::radians(m_pitch));
		forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		forward = glm::normalize(forward);

		transform->rotation = glm::degrees(glm::eulerAngles(glm::quatLookAt(forward, glm::vec3(0.f, 1.f, 0.f))));

		m_previousCameraX = aimAction->x;
		m_previousCameraY = aimAction->y;

		// todo #13: there's no reason for us ton control multiple cameras, we should figure out how to handle a single camera.
		break;
	}
}
