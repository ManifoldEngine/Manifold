#include "DebugCameraControllerSystem.h"
#include <Camera/CameraSystem.h>
#include <ECS/Entity.h>
#include <ECS/RegistryView.h>
#include <Core/Components/Transform.h>
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/Log.h>
#include <OpenGL/OpenGLSystem.h>

#include <memory>
#include <glm/gtc/quaternion.hpp>

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
	std::weak_ptr<const OpenGLSystem> weakOpenGLSystem = Application::get().getSystemContainer().getSystem<OpenGLSystem>();
	if (weakOpenGLSystem.expired())
	{
		return;
	}

	std::shared_ptr<const OpenGLSystem> openGLSystem = weakOpenGLSystem.lock();
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
