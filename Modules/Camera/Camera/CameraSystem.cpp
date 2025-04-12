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

void CameraSystem::onInitialize(ECS::Registry& registry, World& world)
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
    for (const ECS::EntityId& entityId : ECS::View<Transform, Camera>(registry))
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
        
        switch (camera->mode)
        {
        case Camera::EMode::PERSPECTIVE:
            {
                MANI_ASSERT(Math::abs(camera->height) > FLT_EPSILON, "Height of a camera cannot be 0.");
                camera->projection = Mat4f::perspective(Math::degToRad(camera->fov),
                                                        camera->width / camera->height,
                                                        camera->nearClipPlane, 
                                                        camera->farClipPlane);
                break;
            }

            case Camera::EMode::ORTHOGRAPHIC:
            {
                camera->projection = Mat4f::orthographic(0.f, camera->width, 0.f, camera->height, camera->nearClipPlane, camera->farClipPlane);
                break;
            }

            default:
            {
                MANI_ASSERT(false, "Unsupported camera mode.");
                break;
            }
        }
    }
}

Vec2f Mani::CameraSystem::worldToScreenSpace(const ECS::Registry& registry, const Vec3f& position)
{
    if (const Camera* camera = getCamera(registry))
    {
        return camera->worldToScreenSpace(position);
    }
    return VEC2F::ZERO;
}

Vec3f Mani::CameraSystem::screenToWorldSpace(const ECS::Registry& registry, const Vec2f& position)
{
    if (const Camera* camera = getCamera(registry))
    {
        return camera->screenToWorldSpace(position);
    }
    return VEC3F::ZERO;
}

Vec3f Mani::CameraSystem::screenToWorldProjection(const ECS::Registry& registry, const Vec2f& position, float distance)
{
    const Camera* camera = getCamera(registry);
    if (camera == nullptr)
    {
        return VEC4F::ZERO;
    }

    if (camera->mode != Camera::EMode::PERSPECTIVE)
    {
        MANI_LOG_WARNING(LogCamera, "Trying to do Screen to World projection with a non perspective camera");
        return VEC4F::ZERO;
    }

    const Transform* transform = getTransform(registry);
    if (transform == nullptr)
    {
        return VEC4F::ZERO;
    }

    const float theta = Math::degToRad(camera->fov) * .5f;
    const float aspectRatio = camera->getAspectRatio();
    
    const float tanHalfTheta = Math::tan(theta * .5f);
    const float yMin = tanHalfTheta * distance;
    const float yMax = -tanHalfTheta * distance;
    const float xMax = yMax * aspectRatio;
    const float xMin = yMin * aspectRatio;

    MANI_ASSERT(!Math::isEqual(camera->width, 0.f), "divide by zero");
    const float xRatio = position.x / (camera->width * .5f);
    MANI_ASSERT(!Math::isEqual(camera->height, 0.f), "divide by zero");
    const float yRatio = position.y / (camera->height * .5f);

    const float xOffset = ((xMax - xMin) * .5f) * xRatio;
    const float yOffset = ((yMax - yMin) * .5f) * yRatio;

    return transform->position +
        transform->right() * xOffset +
        transform->up() * yOffset +
        transform->forward() * distance;
}

const Camera* Mani::CameraSystem::getCamera(const ECS::Registry& registry)
{
    if (const CameraSystemCache* cache = registry.getSingle<CameraSystemCache>())
    {
        return registry.get<Camera>(cache->cameraId);
    }
    return nullptr;
}

const Transform* Mani::CameraSystem::getTransform(const ECS::Registry& registry)
{
    if (const CameraSystemCache* cache = registry.getSingle<CameraSystemCache>())
    {
        return registry.get<Transform>(cache->cameraId);
    }
    return nullptr;
}
