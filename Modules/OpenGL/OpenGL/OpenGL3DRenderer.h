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
	public:
		virtual std::string_view getName() const override { return "OpenGL3DRendererSystem"; }

	protected:
		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry, World& world);

		OpenGL3DRenderer renderer;
	};
}