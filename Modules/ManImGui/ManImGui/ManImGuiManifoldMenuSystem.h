#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiManifoldMenuSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiManifoldMenuSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    private:
        struct Storage;
    };
}