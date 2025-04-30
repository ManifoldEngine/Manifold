#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class OpenGLCameraUpdateSystem : public Mani::ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "OpenGLCameraUpdateSystem"; }
        virtual bool shouldTick(Mani::ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender; }

        virtual void tick(float deltaTime, Mani::ECS::Registry& registry) override;
    };
}