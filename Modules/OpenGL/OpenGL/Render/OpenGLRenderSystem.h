#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class OpenGLRenderSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "OpenGLRenderSystem"; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::End; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry) override;

    private:
        struct Storage;
    };
}