#pragma once

#include <Core/CoreFwd.h>
#include <OpenGL/Render/IOpenGLRenderer.h>

namespace Mani
{
    class OpenGLUIRenderer : public IOpenGLRenderer
    {
    public:
        // Inherited via IOpenGLRenderer
        int getId() const override;
        virtual void onBegin(OpenGLRenderContext& context) override;
        virtual void render(const OpenGLCommand& command, OpenGLRenderContext& context) override;
        virtual void onEnd(OpenGLRenderContext& context) override;
    };

    class OpenGLUIRendererSystem : public ECS::System
    {
    public:
        virtual std::string_view getName() const override { return "RendererUISystem"; }
        virtual bool shouldTick(const ECS::Registry& registry) const override { return false; }

    protected:
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

        OpenGLUIRenderer m_renderer;
    };
}