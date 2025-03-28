#include "CameraSystem.h"

#include <Core/Components/Transform.h>
#include <Core/ManiAssert.h>

#include <ECS/Entity.h>
#include <ECS/Registry.h>
#include <ECS/View.h>

using namespace Mani;

struct CameraSystemCache
{
    ECS::EntityId cameraId; 
};

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
    CameraSystemCache& cache = *registry.addSingle<CameraSystemCache>();

    cache.cameraId = registry.create();
    Transform* transform = registry.add<Transform>(cache.cameraId);
    transform->position = Vec3f(0.0f, 0.0f, -3.0f);
    registry.add<Camera>(cache.cameraId);
}

void CameraSystem::onDeinitialize(ECS::Registry& registry)
{
    if (const CameraSystemCache* cache = registry.getSingle<CameraSystemCache>())
    {
        registry.destroy(cache->cameraId);
    }
    registry.removeSingle<CameraSystemCache>();
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
