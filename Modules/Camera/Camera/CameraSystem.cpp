#include "CameraSystem.h"

#include <Core/Transform.h>
#include <Core/ManiAssert.h>
#include <Core/Vec.h>

#include <ECS/Entity.h>
#include <ECS/Registry.h>
#include <ECS/View.h>

using namespace Mani;

struct CameraSystem::Storage
{
    ECS::EntityId cameraId; 
};

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
    Storage& storage = *registry.addSingle<Storage>();

    storage.cameraId = registry.create();
    Position* position = registry.add<Position>(storage.cameraId);
    position->value = VEC3F::BACK * 5.f; // film the origin by default
    registry.add<Rotation>(storage.cameraId);
    Camera& camera = *registry.add<Camera>(storage.cameraId);
    
    const CoreConfig& config = Application::get().getConfig();
    const float width = static_cast<float>(config.startupScreenWidth);
    const float height = static_cast<float>(config.startupScreenHeight);

    camera.width = width;
    camera.height = height;
    camera.virtualWidth = width;
    camera.virtualHeight = height;
}

void CameraSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
    const Storage& storage = *registry.getSingle<Storage>();
    registry.destroy(storage.cameraId);
    registry.removeSingle<Storage>();
}

void CameraSystem::tick(ECS::Registry& registry)
{
    for (const ECS::EntityId& entityId : ECS::View<Position, Rotation, Camera>(registry))
    {
        Position& position = *registry.get<Position>(entityId);
        Rotation& rotation = *registry.get<Rotation>(entityId);
        Camera& camera = *registry.get<Camera>(entityId);

        camera.view = Mat4f::lookAt(position.value, position.value + Transform::forward(rotation), Transform::up(rotation));
        camera.frustrum = FrustumStatics::create(camera, position.value, rotation.value);

        switch (camera.mode)
        {
            case ECameraMode::PERSPECTIVE:
            {
                MANI_ASSERT(Math::abs(camera.height) > FLT_EPSILON, "Height of a camera cannot be 0.");
                camera.projection = Mat4f::perspective(Math::degToRad(camera.fov),
                                                        camera.width / camera.height,
                                                        camera.near, camera.far);
                break;
            }

            case ECameraMode::ORTHOGRAPHIC:
            {
                MANI_ASSERT(camera.pixelsPerUnit != 0, "Do not divide by zero");
                float ppu = static_cast<float>(camera.pixelsPerUnit);
                if (camera.useVirtualResolution)
                {
                    MANI_ASSERT(!Math::isEqual(camera.virtualWidth, 0.f) && !Math::isEqual(camera.virtualHeight, 0.f), "Do not divide by zero");
                    float ratioX = camera.width / camera.virtualWidth;
                    float ratioY = camera.height / camera.virtualHeight;
                    const float ratio = Math::minT(ratioX, ratioY);
                    ppu *= ratio;
                }

                const float width = Math::floor(camera.width / ppu);
                const float height = Math::floor(camera.height / ppu);
                MANI_ASSERT(!Math::isEqual(width, 0.f) && !Math::isEqual(height, 0.f), "trying to render with a width or height of 0 pixels");
                const float halfWidth = width / 2.f;
                const float halfHeight = height / 2.f;
                camera.projection = Mat4f::orthographic(-halfWidth, halfWidth,
                                                        -halfHeight, halfHeight,
                                                        camera.near, camera.far);
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
