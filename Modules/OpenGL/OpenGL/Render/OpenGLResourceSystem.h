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
        virtual void onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId) const override;
        virtual void onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId) const override;
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

        static void onMeshLoaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onMaterialLoaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onShaderLoaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onSTBITextureLoaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onSpriteLoaded(ECS::Registry& registry, ECS::EntityId entityId);

        static void onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onSTBITextureUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
        static void onSpriteUnloaded(ECS::Registry& registry, ECS::EntityId entityId);
    };

    template<>
    bool ResourceLoader::load<STBITexture>(const std::filesystem::path& absolutePath, Resource<STBITexture>& resource);
}