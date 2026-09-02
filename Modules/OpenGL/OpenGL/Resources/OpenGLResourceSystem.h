#pragma once

#include <Core/CoreFwd.h>
#include <Resources/IResourceSystemExtension.h>
#include <Resources/ResourceSystem.h>
#include <STB/Textures/ResourceLoader_Texture.h>

namespace Mani
{
    class OpenGLResourceSystemExtension : public IResourceSystemExtension
    {
        // this is all guarranteed to be called from the main thread.
        virtual void onResourceLoaded(ECS::Registry& registry, EntityId entityId, uint32_t tag) const override;
        virtual void onResourceUnloaded(ECS::Registry& registry, EntityId entityId, uint32_t tag) const override;
    };

    class OpenGLResourceSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "OpenGLResourceSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    private:
        OpenGLResourceSystemExtension m_resourceExtension;
        ResourceLoader_Texture m_textureLoader;
    };
}