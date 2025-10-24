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

		// called when starting to receive commands
		virtual void onBegin(OpenGLRenderContext& context) = 0;
		// render one command
		virtual void render(const OpenGLCommand& command, OpenGLRenderContext& context) = 0;
		// called when done receiving commands
		virtual void onEnd(OpenGLRenderContext& context) = 0;
	};
}