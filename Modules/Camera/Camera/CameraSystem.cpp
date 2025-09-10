#include "CameraSystem.h"

#include <Core/Components/Transform.h>
#include <Core/ManiAssert.h>
#include <Core/Vec.h>

#include <ECS/Entity.h>
#include <ECS/Registry.h>
#include <ECS/View.h>

using namespace Mani;

struct CameraSystemCache
{
    ECS::EntityId cameraId; 
};

const Camera* getCamera(const ECS::Registry& registry)
{
    if (const CameraSystemCache* cache = registry.getSingle<CameraSystemCache>())
    {
        return registry.get<Camera>(cache->cameraId);
    }
    return nullptr;
}

std::tuple<const Position*, const Rotation*> getCameraPositionAndRotation(const ECS::Registry& registry)
{
    if (const CameraSystemCache* cache = registry.getSingle<CameraSystemCache>())
    {
        return { registry.get<Position>(cache->cameraId), registry.get<Rotation>(cache->cameraId) };
    }
    return { nullptr, nullptr };
}

std::string_view CameraSystem::getName() const
{
    return "CameraSystem";
}

bool CameraSystem::shouldTick(const ECS::Registry& registry) const
{
    return true;
}

void CameraSystem::onInitialize(ECS::Registry& registry, World& world)
{
    CameraSystemCache& cache = *registry.addSingle<CameraSystemCache>();

    cache.cameraId = registry.create();
    Position* position = registry.add<Position>(cache.cameraId);
    position->value = Mani::VEC3F::BACK * 5.f; // film the origin by default
    registry.add<Rotation>(cache.cameraId);
    registry.add<Camera>(cache.cameraId);
}

void CameraSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
    if (const CameraSystemCache* cache = registry.getSingle<CameraSystemCache>())
    {
        registry.destroy(cache->cameraId);
    }
    registry.removeSingle<CameraSystemCache>();
}

void CameraSystem::tick(ECS::Registry& registry)
{
    for (const ECS::EntityId& entityId : ECS::View<Position, Rotation, Camera>(registry))
    {
        Position& position = *registry.get<Position>(entityId);
        Rotation& rotation = *registry.get<Rotation>(entityId);
        Camera& camera = *registry.get<Camera>(entityId);

        camera.view = Mat4f::lookAt(position.value, position.value + Transform::forward(rotation), Transform::up(rotation));
        
        switch (camera.mode)
        {
        case Camera::EMode::PERSPECTIVE:
            {
                MANI_ASSERT(Math::abs(camera.height) > FLT_EPSILON, "Height of a camera cannot be 0.");
                camera.projection = Mat4f::perspective(Math::degToRad(camera.fov),
                                                        camera.width / camera.height,
                                                        camera.nearClipPlane, 
                                                        camera.farClipPlane);
                break;
            }

            case Camera::EMode::ORTHOGRAPHIC:
            {
                const float zoomFactor = camera.orthographicZoomFactor;
                const float halfWidth = camera.width * zoomFactor / 2.f;
                const float halfHeight = camera.height * zoomFactor / 2.f;
                camera.projection = Mat4f::orthographic(-halfWidth, halfWidth,
                                                        -halfHeight, halfHeight,
                                                        camera.nearClipPlane, camera.farClipPlane);
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

Vec3f Mani::CameraSystem::screenToWorldProjection(const ECS::Registry& registry, const Vec2f& screenPosition, float distance)
{
    const Camera* camera = getCamera(registry);
    if (camera == nullptr)
    {
        return VEC3F::ZERO;
    }

    const Vec2f position {
        Math::clamp(screenPosition.x, 0.f, camera->width), 
        Math::clamp(screenPosition.y, 0.f, camera->height)
    };

    switch (camera->mode)
    {
        case Camera::EMode::PERSPECTIVE:
        {
            auto [cameraPosition, cameraRotation] = getCameraPositionAndRotation(registry);
            if (cameraPosition == nullptr || cameraRotation == nullptr)
            {
                return VEC3F::ZERO;
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

            return cameraPosition->value +
                Transform::right(*cameraRotation) * xOffset +
                Transform::up(*cameraRotation) * yOffset +
                Transform::forward(*cameraRotation) * distance;
        }

        case Camera::EMode::ORTHOGRAPHIC:
        {
            auto [cameraPosition, cameraRotation] = getCameraPositionAndRotation(registry);
            if (cameraPosition == nullptr || cameraRotation == nullptr)
            {
                return VEC3F::ZERO;
            }

            const float halfWidth = camera->width * 0.5f;
            const float halfHeight = camera->height * 0.5f;

            Vec2f centeredPosition = position - Vec2f{ halfWidth, halfHeight };
            // we reverse the height because the screen coordinates go from top to bottom
            centeredPosition.y *= -1.f;

            const Vec3f& cameraPositionValue = cameraPosition->value;
            Vec3f worldPosition = cameraPositionValue + static_cast<Vec3f>(centeredPosition * camera->orthographicZoomFactor);
            worldPosition.z = cameraPositionValue.z;
            return cameraRotation->value.rotate(worldPosition);
        }

        default: 
            MANI_ASSERT(false, "Unsupported camera mode.");
    }

    return VEC3F::ZERO;
}
