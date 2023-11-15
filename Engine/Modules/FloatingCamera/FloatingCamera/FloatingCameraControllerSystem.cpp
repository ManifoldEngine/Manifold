#include "FloatingCameraControllerSystem.h"

#include <Camera/CameraSystem.h>
#include <ECS/Entity.h>
#include <ECS/RegistryView.h>
#include <Core/Components/Transform.h>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Log.h>
#include <Inputs/InputSystem.h>


using namespace ECSEngine;

std::string_view FloatingCameraControllerSystem::getName() const
{
	return "FloatingCameraControllerSystem";
}

bool FloatingCameraControllerSystem::shouldTick(EntityRegistry& registry) const
{
	return true;
}

void FloatingCameraControllerSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
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

void FloatingCameraControllerSystem::tick(float deltaTime, ECSEngine::EntityRegistry& registry)
{
	if (m_inputSystem.expired())
	{
		return;
	}

	std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();

	const InputAction* moveAction = inputSystem->getAction(LOCAL_USERID, MOVE_ACTION);
	const InputAction* aimAction = inputSystem->getAction(LOCAL_USERID, AIM_ACTION);

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

		transform->position +=	(transform->right() * static_cast<float>(moveAction->x) + 
								transform->up() * static_cast<float>(moveAction->y) +
								transform->forward() * static_cast<float>(moveAction->z))  * 
								deltaTime * CAMERA_SPEED;

		const float aimX = static_cast<float>(aimAction->x);
		const float aimY = static_cast<float>(aimAction->y);

		const float deltaX = m_previousCameraX - aimX;
		const float deltaY = m_previousCameraY - aimY;

		m_yaw += deltaX * m_sensitivity;
		m_pitch += deltaY * m_sensitivity;

		glm::quat qPitch = glm::angleAxis(glm::radians(-m_pitch), glm::vec3(1.f, 0.f, 0.f));
		glm::quat qYaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0.f, 1.f, 0.f));
		
		transform->rotation = qYaw * qPitch;

		m_previousCameraX = aimX;
		m_previousCameraY = aimY;

		// todo #13: there's no reason for us ton control multiple cameras, we should figure out how to handle a single camera.
		break;
	}
}
