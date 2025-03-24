#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiSystem : public Mani::SystemBase
    {
#if MANI_DEBUG
    public:
        virtual std::string_view getName() const override { return "ManImGuiSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const { return ETickGroup::PostTick; }

        virtual void tick(float deltaTime, Mani::ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, Mani::SystemContainer& systemContainer) override;
        virtual void onDeinitialize(ECS::Registry& registry) override;
#endif
    };
}