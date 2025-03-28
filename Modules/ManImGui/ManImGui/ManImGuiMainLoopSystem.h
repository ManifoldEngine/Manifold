#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    struct IsManImGuiDisplayed;
    struct InputAction;

    class ManImGuiMainLoopSystem : public Mani::SystemBase
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer);
        virtual void onDeinitialize(ECS::Registry& registry);

    private:
        void handleInputs(ECS::Registry& registry, IsManImGuiDisplayed& isManImGuiDisplayed, const InputAction& toggleManImGui);
    };
}