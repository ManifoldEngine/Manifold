#pragma once

#include <Core/CoreFwd.h>
#include <Resources/IResourceSystemExtension.h>
#include <Resources/ResourceSystem.h>

namespace Mani
{
    class OpenGLResourceSystemExtension : public IResourceSystemExtension
    {
        // this is all guarranteed to be called from the main thread.
        virtual void onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const override;
        virtual void onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const override;
    };

    class OpenGLResourceSystem : public ECS::System
    {
    public:
        friend OpenGLResourceSystemExtension;

        virtual std::string_view getName() const override { return "OpenGLResourceSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    private:
        struct Storage;
        OpenGLResourceSystemExtension resourceExtension;

        static void onMeshLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onMaterialLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onShaderLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onTextureLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        
        static void onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onTextureUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onTexture2DUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
    };
}