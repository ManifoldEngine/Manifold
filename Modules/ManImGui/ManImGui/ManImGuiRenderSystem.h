#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class ManImGuiRenderSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "ManImGuiRenderSystem"; }

        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Render; }
        virtual void tick(float deltaTime, ECS::Registry& registry) override;
    };
}