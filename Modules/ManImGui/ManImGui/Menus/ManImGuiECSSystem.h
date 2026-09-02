#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiECSSystem : public ECS::SystemT<ManImGuiECSSystem, ETick::Enabled>
    {
        virtual bool shouldTick(const ECS::Registry& registry) const override;
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void tick(ECS::Registry& registry) override;
    };
}