#include "UI.h"

#include <Camera/Camera.h>

#include <RenderAPI/Components/MeshRendering.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/Sprite.h>
#include <RenderAPI/RendererIds.h>

#include <UI/Components/FillableBar.h>

using namespace Mani;

Ref<const Camera> Mani::UI::getUICamera(const ECS::Registry& registry)
{
    auto context = registry.getSingle<UI::Context>();
    return registry.get<Camera>(context->cameraId);
}

ECS::EntityId UI::createSprite(ECS::Registry& registry, const Vec2f& size, ECS::EntityId textureId, ECS::EntityId materialId, uint32_t tag)
{
    ECS::EntityId entityId = registry.create();
    registry.addMany<Position, Rotation, Scale>(entityId);

    constexpr bool withBoundingSphere = false;
    Sprites::addRendering(registry, entityId, size, textureId, materialId, tag, withBoundingSphere);

    Ref<MeshRendering> meshRendering = registry.get<MeshRendering>(entityId);
    meshRendering->rendererId = Mani::RendererIds::UI_PASS;
    return entityId;
}

ECS::EntityId UI::createFillableBar(ECS::Registry& registry, const Vec2f& size, ECS::EntityId textureId, ECS::EntityId materialId, const Vec4f& backgroundColor, uint32_t tag)
{
    ECS::EntityId entityId = registry.create();
    registry.addMany<Position, Rotation, Scale>(entityId);

    constexpr bool withBoundingSphere = false;
    Sprites::addRendering(registry, entityId, size, textureId, materialId, tag, withBoundingSphere);

    Ref<MeshRendering> meshRendering = registry.get<MeshRendering>(entityId);
    meshRendering->shaderParameters[Mani::UI::ShaderNames::MANI_UI_FILLRATIO] = 0.f;
    meshRendering->shaderParameters[Mani::ShaderNames::MANI_COLOR] = backgroundColor;
    meshRendering->rendererId = Mani::RendererIds::UI_PASS;
    
    registry.add<FillableBar>(entityId, 0.f);
    return entityId;
}

Vec3f UI::ndcToUISpace(const Vec2f& ndc, const Camera& uiCamera)
{
    MANI_ASSERT(!Math::isEqual(uiCamera.pixelsPerUnit, 0.f), "Invalid UI Camera with a ppu of 0");

    const Vec2f dimensions = Vec2f{ uiCamera.virtualWidth, uiCamera.virtualHeight} / static_cast<float>(uiCamera.pixelsPerUnit);
    return ndcToUISpace(ndc, dimensions);
}

Vec3f UI::ndcToUISpace(const Vec2f& ndc, const Vec2f& dimensions)
{
    return Vec3f{ 
        ndc.x * (dimensions.x * .5f),
        ndc.y * (dimensions.y * .5f),
        0.f
    };
}
