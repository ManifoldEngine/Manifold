#pragma once

#include <Core/CoreFwd.h>
#include <Core/Async/ThreadPool.h>

#include <OpenGL/Render/IOpenGLRenderer.h>
#include <OpenGL/Render/IOpenGLRenderExtension.h>
#include <OpenGL/Render/OpenGLRenderStorage.h>

#include <string_view>

namespace Mani
{
	const std::string_view LogOpenGL = "OpenGL";
	const std::string_view LogOpenGLAPI = "OPENGL_CALLBACK";

	namespace OpenGL
	{
		// Enqueues a task to run on the render thread.
		// Used to safely schedule OpenGL work from other threads.
		template<typename TFunctor, typename... TArgs>
		void enqueueRenderTask(ECS::Registry& registry, TFunctor&& f, TArgs&&... args);

		// Registers an OpenGL render extension (e.g. debug tools, overlays).
		void registerExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension);
		// Unregisters a previously registered render extension.
		void unregisterExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension);

		// Registers a renderer that handles specific OpenGL draw commands.
		void registerRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer);
		// Unregisters a previously registered renderer.
		void unregisterRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer);
	}

	template<typename TFunctor, typename ...TArgs>
	void OpenGL::enqueueRenderTask(ECS::Registry& registry, TFunctor&& f, TArgs && ...args)
	{
		if (OpenGLRenderStorage* storage = registry.getSingle<OpenGLRenderStorage>())
		{
			storage->renderThread.enqueue(std::forward<TFunctor>(f), std::forward<TArgs>(args)...);
		}
	}
}