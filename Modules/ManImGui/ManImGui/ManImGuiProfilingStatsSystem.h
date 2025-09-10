#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiProfilingStatsSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiProfilingStatsSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const { return ETickGroup::Update; };

        virtual void tick(ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
    };
}