#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class FillableBarSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "FillableBarSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender; }

    public:
        virtual void tick(ECS::Registry& registry) override;
    };
}