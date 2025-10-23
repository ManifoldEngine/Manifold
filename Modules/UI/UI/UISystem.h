#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class UISystem : public Mani::ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "UISystem"; }
        virtual bool shouldTick(const Mani::ECS::Registry& registry) const override { return true; }
        virtual Mani::ETickGroup getTickGroup() const override { return Mani::ETickGroup::PreRender; }

    protected:
        virtual void onInitialize(Mani::ECS::Registry& registry, Mani::World& world) override;
        virtual void onDeinitialize(Mani::ECS::Registry& registry, Mani::World& world) override;

    public:
        virtual void tick(Mani::ECS::Registry& registry) override;
    };
}