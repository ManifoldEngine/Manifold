#include "CameraSystem.h"

#include <Core/Components/Transform.h>
#include <Core/ManiAssert.h>

#include <ECS/EntityRegistry.h>
#include <ECS/RegistryView.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>

using namespace Mani;

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

        cameraComponent->view = glm::lookAt(transform->position, transform->position + transform->forward(), transform->up());
        
        const CameraConfig& config = cameraComponent->config;
        MANI_ASSERT(std::abs(config.height) > FLT_EPSILON, "Height of a camera cannot be 0.");
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

Transform* CameraSystem::getCameraTransform(EntityRegistry& registry) const
{
    return registry.getComponent<Transform>(m_cameraId);
}

void CameraSystem::setCameraConfig(EntityRegistry& registry, const CameraConfig& config)
{
    if (auto* cameraComponent = registry.getComponent<CameraComponent>(m_cameraId))
    {
        cameraComponent->config = config;
    }
}

glm::vec2 CameraSystem::worldToScreenSpace(glm::vec3 position, const EntityRegistry& registry) const
{
    const CameraComponent* cameraComponent = getCameraComponent(registry);
    if (cameraComponent == nullptr)
    {
        return glm::vec2();
    }

    glm::vec4 position4 = glm::vec4(position, 1.0f);
    glm::vec4 projectedPosition = position4 * cameraComponent->view;
    
    if (std::abs(projectedPosition.w) <= FLT_EPSILON)
    {
        return glm::vec2();
    }

    return glm::vec2(projectedPosition.x / projectedPosition.w, projectedPosition.y / projectedPosition.w);
}

float Mani::CameraConfig::getAspectRatio() const
{
    MANI_ASSERT(std::abs(height) > FLT_EPSILON, "height cannot be zero");
    return width / height;
}
