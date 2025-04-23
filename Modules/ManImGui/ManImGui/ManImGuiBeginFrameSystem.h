#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    struct IsManImGuiDisplayed;
    struct InputAction;

    class ManImGuiBeginFrameSystem : public Mani::ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world);
        virtual void onDeinitialize(ECS::Registry& registry);
    };
}