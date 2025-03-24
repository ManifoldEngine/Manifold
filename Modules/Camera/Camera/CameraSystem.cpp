#include "CameraSystem.h"

#include <Core/Components/Transform.h>
#include <Core/ManiAssert.h>

#include <ECS/Entity.h>
#include <ECS/Registry.h>
#include <ECS/View.h>

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
    ECS::EntityId cameraId = registry.create();
    Transform* transform = registry.add<Transform>(cameraId);
    transform->position = Vec3f(0.0f, 0.0f, -3.0f);
    registry.add<Camera>(cameraId);
}

void CameraSystem::onDeinitialize(ECS::Registry& registry)
{
    ECS::View<Transform, Camera> cameraView(registry);
    auto it = cameraView.begin();
    if (it != cameraView.end())
    {
        registry.destroy(*it);
    }
}

void CameraSystem::tick(float deltaTime, ECS::Registry& registry)
{
    ECS::View<Transform, Camera> view(registry);
    for (const ECS::EntityId& entityId : view)
    {
        Transform* transform = registry.get<Transform>(entityId);
        if (transform == nullptr)
        {
            continue;
        }

        Camera* camera = registry.get<Camera>(entityId);
        if (camera == nullptr)
        {
            continue;
        }

        camera->view = Mat4f::lookAt(transform->position, transform->position + transform->forward(), transform->up());
        
        MANI_ASSERT(Math::abs(camera->height) > FLT_EPSILON, "Height of a camera cannot be 0.");
        camera->projection = Mat4f::perspective(Math::degToRad(camera->fov),
                                                         camera->width / camera->height,
                                                         camera->nearClipPlane, 
                                                         camera->farClipPlane);
    }
}
