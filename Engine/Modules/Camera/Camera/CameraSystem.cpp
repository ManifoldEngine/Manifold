#include "CameraSystem.h"
#include <ECS/EntityRegistry.h>
#include <Core/Components/Transform.h>
#include <ECS/RegistryView.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <Core/CoreAssert.h>
#include <algorithm>

using namespace ECSEngine;

std::string_view CameraSystem::getName() const
{
    return "CameraSystem";
}

bool CameraSystem::shouldTick(EntityRegistry& registry) const
{
    return true;
}

void CameraSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    m_cameraId = registry.create();
    Transform* transform = registry.addComponent<Transform>(m_cameraId);
    transform->position = glm::vec3(0.0f, 0.0f, -3.0f);
    registry.addComponent<CameraComponent>(m_cameraId);
}

void CameraSystem::onDeinitialize(EntityRegistry& registry)
{
    registry.destroy(m_cameraId);
}

void CameraSystem::tick(float deltaTime, EntityRegistry& registry)
{
    RegistryView<Transform, CameraComponent> view(registry);
    for (const EntityId& entityId : view)
    {
        Transform* transform = registry.getComponent<Transform>(entityId);
        if (transform == nullptr)
        {
            continue;
        }

        CameraComponent* cameraComponent = registry.getComponent<CameraComponent>(entityId);
        if (cameraComponent == nullptr)
        {
            continue;
        }

        cameraComponent->view = transform->calculate();
        
        const CameraConfig& config = cameraComponent->config;
        ECSE_ASSERT(std::abs(config.height) > FLT_EPSILON, "Height of a camera cannot be 0.");
        cameraComponent->projection = glm::perspective(glm::radians(config.fov), 
                                                        config.width / config.height, 
                                                        config.nearClipPlane, 
                                                        config.farClipPlane);
    }
}

const CameraComponent* CameraSystem::getCameraComponent(const EntityRegistry& registry) const
{
    return registry.getComponent<CameraComponent>(m_cameraId);
}

void CameraSystem::setCameraConfig(EntityRegistry& registry, const CameraConfig& config)
{
    if (auto* cameraComponent = registry.getComponent<CameraComponent>(m_cameraId))
    {
        cameraComponent->config = config;
    }
}
