#pragma once

#include <Core/CoreFwd.h>
#include <Resources/IResourceSystemExtension.h>

namespace Mani
{
    struct ResourceHandle;

    class OpenGLResourceSystemExtension : public IResourceSystemExtension
    {
        virtual void onResourceCreated(ECS::Registry& registry, ECS::EntityId entityId) const override;
        virtual void onResourceDestroyed(ECS::Registry& registry, ECS::EntityId entityId) const override;
    };

    class OpenGLResourceSystem : public ECS::System
    {
    public:
        friend OpenGLResourceSystemExtension;

        virtual std::string_view getName() const override { return "OpenGLResourceSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return false; }

        static ECS::EntityId getOpenGLResourceId(ECS::Registry& registry, ECS::EntityId entityId);

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry) override;

    private:
        struct Storage;

        static void onMeshLoaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onMaterialLoaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onSpriteLoaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onShaderLoaded(ECS::Registry& registry, ECS::EntityId entityId);

        static void onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onSpriteUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
    };
}