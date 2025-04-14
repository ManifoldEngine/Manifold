#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class InputSystem : public SystemBase
    {
    public:
        virtual std::string_view getName() const override { return "InputSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Input; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world);
        virtual void onDeinitialize(ECS::Registry& registry);
    };
}