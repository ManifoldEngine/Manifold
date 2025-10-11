#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiTopMenuBarSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiTopMenuBarSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override;
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Update; }

    public:
        virtual void tick(ECS::Registry& registry) override;
    };
}