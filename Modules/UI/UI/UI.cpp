#include "UI.h"

#include <Camera/Camera.h>

#include <RenderAPI/MeshRendering.h>
#include <RenderAPI/Shader.h>

#include <Sprite/Sprite.h>

#include <UI/Components/FillableBar.h>

using namespace Mani;

ECS::EntityId UI::createSprite(ECS::Registry& registry, const std::string_view& spritePath, uint8_t tag, const Vec4f& color)
{
    ECS::EntityId entityId = registry.create();
    registry.add<MeshRendering>(entityId);
    registry.addMany<Position, Rotation, Scale>(entityId);
    constexpr bool withBoundingSphere = false;
    SpriteStatics::loadAsyncAndAddSprite(registry, entityId, spritePath, UI::ShaderPaths::UI_SPRITE, tag, withBoundingSphere);
    
    Ref<MeshRendering> meshRendering = registry.get<MeshRendering>(entityId);
    meshRendering->shaderParameters[Mani::ShaderNames::MANI_COLOR] = color;
    meshRendering->rendererId = Mani::UI::Constants::RENDERER_ID;
    return entityId;
}

ECS::EntityId UI::createFillableBar(ECS::Registry& registry, const std::string_view& spritePath, uint8_t tag, const Vec4f& backgroundColor)
{
    ECS::EntityId entityId = registry.create();
    registry.add<MeshRendering>(entityId);
    registry.addMany<Position, Rotation, Scale>(entityId);
    constexpr bool withBoundingSphere = false;
    SpriteStatics::loadAsyncAndAddSprite(registry, entityId, spritePath, UI::ShaderPaths::UI_FILLABLEBAR, tag, withBoundingSphere);

    Ref<MeshRendering> meshRendering = registry.get<MeshRendering>(entityId);
    meshRendering->shaderParameters[Mani::UI::ShaderNames::MANI_UI_FILLRATIO] = 0.f;
    meshRendering->shaderParameters[Mani::ShaderNames::MANI_COLOR] = backgroundColor;
    meshRendering->rendererId = Mani::UI::Constants::RENDERER_ID;

    registry.add<FillableBar>(entityId, 0.f);

    return entityId;
}

Vec3f UI::ndcToUISpace(const ECS::Registry& registry, const Vec2f& ndc)
{
    Ref<const UI::Context> context = registry.findSingle<UI::Context>();
    MANI_ASSERT(context.isValid(), "Trying to convert coordinates without a UI context");
    Ref<const Camera> camera = registry.get<Camera>(context->cameraId);

    MANI_ASSERT(!Math::isEqual(camera->pixelsPerUnit, 0.f), "Invalid UI Camera with a ppu of 0");
    
    const Vec2f dimensions = Vec2f{ camera->virtualWidth, camera->virtualHeight} / static_cast<float>(camera->pixelsPerUnit);
    return ndcToUISpace(ndc, dimensions);
}

Vec3f UI::ndcToUISpace(const Vec2f& ndc, const Vec2f& dimensions)
{
    MANI_ASSERT(!Math::isEqual(dimensions.x, 0.f), "Do not divide by 0");
    MANI_ASSERT(!Math::isEqual(dimensions.y, 0.f), "Do not divide by 0");
    return Vec3f{ 
        ndc.x * (dimensions.x * .5f),
        ndc.y * (dimensions.y * .5f),
        0.f
    };
}
