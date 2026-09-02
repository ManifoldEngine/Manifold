#pragma once

#include <Core/CoreFwd.h>
#include <FMod/Resources/ResourceLoader_FModSound.h>
#include <FMod/Resources/ResourceLoader_FModStream.h>
#include <Resources/IResourceSystemExtension.h>

namespace Mani
{
    class FModResourceExtension : public IResourceSystemExtension
    {
    public:
        virtual void onResourceLoaded(ECS::Registry& registry, EntityId entityId, uint32_t tag) const override;
        virtual void onResourceUnloaded(ECS::Registry& registry, EntityId entityId, uint32_t tag) const override;
    };

    class FModResourceSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "FModResourceSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    private:
        ResourceLoader_FModSound soundLoader;
        ResourceLoader_FModStream streamLoader;
        FModResourceExtension extension;
    };
}