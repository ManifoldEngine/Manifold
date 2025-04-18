#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiProfilingStatsSystem : public Mani::ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiProfilingStatsSystem"; }
        virtual bool shouldTick(Mani::ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, Mani::ECS::Registry& registry) override;
    };
}