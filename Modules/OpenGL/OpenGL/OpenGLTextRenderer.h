#pragma once

#include <Core/CoreFwd.h>
#include <OpenGL/Render/IOpenGLRenderer.h>

namespace Mani
{
    class OpenGLTextRenderer : public IOpenGLRenderer
    {
    public:
        // Inherited via IOpenGLRenderer
        int getId() const override;
        virtual void onBegin(OpenGLRenderContext& context) override;
        virtual void render(const OpenGLCommand& command, OpenGLRenderContext& context) override;
        virtual void onEnd(OpenGLRenderContext& context) override;
    };

    class OpenGLTextRendererSystem : public ECS::SystemT<OpenGLTextRendererSystem, ETick::Disabled>
    {
        virtual void onInitialize(ECS::Registry& registry, World& world) override;
        virtual void onDeinitialize(ECS::Registry& registry, World& world) override;
    private:
        OpenGLTextRenderer m_renderer;
    };
}