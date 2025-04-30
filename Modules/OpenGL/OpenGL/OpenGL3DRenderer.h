#pragma once

#include <ECS/System.h>
#include <OpenGL/Render/IOpenGLRenderer.h>

namespace Mani
{
	class OpenGL3DRenderer : public IOpenGLRenderer
	{
		// Inherited via IRenderer
		virtual int getId() const override { return 0; }
		virtual void render(const OpenGLCommand& command, OpenGLRenderContext& context) override;
	};

	class OpenGL3DRendererSystem : public ECS::System
	{
	protected:
		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry);

		OpenGL3DRenderer renderer;
	};
}