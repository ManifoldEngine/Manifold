#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class FModOneShotSystem : public Mani::ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "FModOneShotSystem"; }
        virtual bool shouldTick(const Mani::ECS::Registry& registry) const override { return true; }
        virtual Mani::ETickGroup getTickGroup() const override { return Mani::ETickGroup::Update; }

    public:
        virtual void tick(Mani::ECS::Registry& registry) override;
    };
}