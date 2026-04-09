#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiECSSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiECSSystem"; }
        //virtual bool shouldTick(const ECS::Registry& registry) const override;
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Update; }

    //protected:
    //    virtual void onInitialize(ECS::Registry& registry, World& world) override;

    //public:
    //    virtual void tick(ECS::Registry& registry) override;
    };
}