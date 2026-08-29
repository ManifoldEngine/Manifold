#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiInputDebugSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiInputDebugSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override;
        virtual ETickGroup getTickGroup() const { return ETickGroup::PreInput; }

        virtual void tick(ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
    };
}