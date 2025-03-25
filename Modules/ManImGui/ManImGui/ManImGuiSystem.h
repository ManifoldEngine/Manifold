#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiSystem : public Mani::SystemBase
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, Mani::SystemContainer& systemContainer) override;
        virtual void onDeinitialize(ECS::Registry& registry) override;
    };
}