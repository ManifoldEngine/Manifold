#pragma once

namespace Mani
{
	struct OpenGLCommand;
	struct OpenGLRenderContext;

	// interface with the render thread
	class IOpenGLRenderer
	{
	public:
		virtual int getId() const = 0;
		virtual void render(const OpenGLCommand& command, OpenGLRenderContext& context) = 0;
	};
}