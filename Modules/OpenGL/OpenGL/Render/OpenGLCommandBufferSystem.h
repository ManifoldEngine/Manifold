#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class OpenGLCommandBufferSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "OpenGLCommandBufferSystem"; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& systemContainer) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;
    };
}