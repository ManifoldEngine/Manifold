#pragma once

#include <Core/CoreFwd.h>

class ManImGuiProfilingStatsSystem : public Mani::SystemBase
{
public:
    virtual std::string_view getName() const override { return "ManImGuiProfilingStatsSystem"; }
    virtual bool shouldTick(Mani::ECS::Registry& registry) const override { return true; }

    virtual void tick(float deltaTime, Mani::ECS::Registry& registry) override;
};