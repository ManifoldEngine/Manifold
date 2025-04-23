#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
    class IOpenGLRenderExtension;

    class OpenGLRenderSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "OpenGLRenderSystem"; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::Render; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

        virtual void tick(float deltaTime, ECS::Registry& registry) override;

        static ECS::EntityId addExtension(ECS::Registry& registry, std::shared_ptr<IOpenGLRenderExtension> extension);
        static void removeExtension(ECS::Registry& registry, ECS::EntityId entityId);

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry) override;

    private:
        struct Storage;
    };
}