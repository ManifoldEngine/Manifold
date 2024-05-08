#include "CameraSystem.h"

#include <Core/Components/Transform.h>
#include <Core/ManiAssert.h>

#include <ECS/Registry.h>
#include <ECS/View.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>

using namespace Mani;

std::string_view CameraSystem::getName() const
{
    return "CameraSystem";
}

bool CameraSystem::shouldTick(ECS::Registry& registry) const
{
    return true;
}

void CameraSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
    m_cameraId = registry.create();
    Transform* transform = registry.add<Transform>(m_cameraId);
    transform->position = glm::vec3(0.0f, 0.0f, -3.0f);
    registry.add<CameraComponent>(m_cameraId);
}

void CameraSystem::onDeinitialize(ECS::Registry& registry)
{
    registry.destroy(m_cameraId);
}

void CameraSystem::tick(float deltaTime, ECS::Registry& registry)
{
    ECS::View<Transform, CameraComponent> view(registry);
    for (const ECS::EntityId& entityId : view)
    {
        Transform* transform = registry.get<Transform>(entityId);
        if (transform == nullptr)
        {
            continue;
        }

        CameraComponent* cameraComponent = registry.get<CameraComponent>(entityId);
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

const CameraComponent* CameraSystem::getCameraComponent(const ECS::Registry& registry) const
{
    return registry.get<CameraComponent>(m_cameraId);
}

Transform* CameraSystem::getCameraTransform(ECS::Registry& registry) const
{
    return registry.get<Transform>(m_cameraId);
}

void CameraSystem::setCameraConfig(ECS::Registry& registry, const CameraConfig& config)
{
    if (auto* cameraComponent = registry.get<CameraComponent>(m_cameraId))
    {
        cameraComponent->config = config;
    }
}

glm::vec2 CameraSystem::worldToScreenSpace(glm::vec3 position, const ECS::Registry& registry) const
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
