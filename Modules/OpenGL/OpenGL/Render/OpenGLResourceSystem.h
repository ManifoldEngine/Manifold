#pragma once

#include <Core/CoreFwd.h>
#include <Resources/IResourceSystemExtension.h>
#include <Resources/ResourceSystem.h>
#include <OpenGL/Data/STBITexture.h>

namespace Mani
{
    struct ResourceHandle;

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
        virtual bool shouldTick(ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry) override;

    private:
        struct Storage;

        static void onMeshLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onMaterialLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onShaderLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onSTBITextureLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onSpriteLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);

        static void onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onSTBITextureUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onTexture2DUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
        static void onSpriteUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag);
    };

    template<>
    bool ResourceLoader::load<STBITexture>(const std::filesystem::path& absolutePath, Resource<STBITexture>& resource);
}