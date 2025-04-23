#pragma once

#include <Core/CoreFwd.h>
#include <OpenGL/Render/IOpenGLRenderExtension.h>

namespace Mani
{
    class ManImGuiRenderSystemExtension : public IOpenGLRenderExtension
    {
        // Inherited via IOpenGLRenderExtension
        void onPostRender(ECS::Registry& registry) const override;
    };

    class ManImGuiRenderSystem : public ECS::System
    {
    public:
        struct Storage;

        virtual std::string_view getName() const override { return "ManImGuiRenderSystem"; }
        virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
        virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender; }
        
        virtual void tick(float deltaTime, ECS::Registry& registry) override;

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world);
        virtual void onDeinitialize(ECS::Registry& registry);
    };
}