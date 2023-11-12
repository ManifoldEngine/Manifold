#include "DebugCameraControllerSystem.h"
#include <Camera/CameraSystem.h>
#include <ECS/Entity.h>
#include <ECS/RegistryView.h>
#include <Core/Components/Transform.h>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Log.h>
#include <Inputs/InputSystem.h>

#include <memory>

using namespace ECSEngine;

std::string_view DebugCameraControllerSystem::getName() const
{
	return "DebugCameraControllerSystem";
}

bool DebugCameraControllerSystem::shouldTick(ECSEngine::EntityRegistry& registry) const
{
	return true;
}

void DebugCameraControllerSystem::tick(float deltaTime, ECSEngine::EntityRegistry& registry)
{
	std::weak_ptr<const InputSystem> weakInputSystem = Application::get().getSystemContainer().getSystem<InputSystem>();
	if (weakInputSystem.expired())
	{
		return;
	}

	std::shared_ptr<const InputSystem> openGLSystem = weakInputSystem.lock();
	GLFWwindow* window = openGLSystem->getWindowContext().window;

	RegistryView<Transform, CameraComponent> view(registry);
	for (const EntityId& entityId : view)
	{
		Transform* transform = registry.getComponent<Transform>(entityId);
		if (transform == nullptr)
		{
			continue;
		}

		const glm::vec3 wasdInput = openGLSystem->getWASDinput();
		const glm::vec2 mouseDelta = openGLSystem->getMouseDelta();

		transform->rotation.y += mouseDelta.x;
		transform->rotation.x += mouseDelta.y;
		transform->position += wasdInput * deltaTime * CAMERA_SPEED;
	}
}
