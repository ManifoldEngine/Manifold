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
    ECS::EntityId cameraId = ECS::INVALID_ID;
};

std::string_view CameraSystem::getName() const
{
    return "CameraSystem";
}

bool CameraSystem::shouldTick(const ECS::Registry& registry) const
{
    return true;
}

void CameraSystem::tick(ECS::Registry& registry)
{
    for (auto [entityId, position, rotation, camera] : ECS::View<Position, Rotation, Camera>(registry))
    {
        camera.view = Mat4f::lookAt(position.value, position.value + Transform::forward(rotation), Transform::up(rotation));
        camera.frustrum = FrustumStatics::create(camera, position.value, rotation.value);

        switch (camera.mode)
        {
            case ECameraMode::Perspective:
            {
                MANI_ASSERT(Math::abs(camera.height) > FLT_EPSILON, "Height of a camera cannot be 0.");
                camera.projection = Mat4f::perspective(Math::degToRad(camera.fov),
                                                        camera.width / camera.height,
                                                        camera.near, camera.far);
                break;
            }

            case ECameraMode::Orthographic:
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
