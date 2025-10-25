#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class FModPlaySystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "FModPlaySystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override;
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Update; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    public:
        virtual void tick(ECS::Registry& registry) override;
    };
}
