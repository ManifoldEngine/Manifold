#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class AnimationSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "AnimationSystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

    public:
        virtual void tick(ECS::Registry& registry) override;
    };
}