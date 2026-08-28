#include "OpenGLRenderSystem.h"

#include <Core/TimeSystem.h>
#include <Core/Profiling/Profiling.h>
#include <Core/Async/ThreadPool.h>

#include <Camera/CameraSystem.h>

#include <RenderAPI/RenderContextSystem.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/OpenGLWindowContext.h>

#include <OpenGL/Resources/OpenGLClearColor.h>

#include <OpenGL/Render/IOpenGLRenderExtension.h>
#include <OpenGL/Render/IOpenGLRenderer.h>
#include <OpenGL/Render/OpenGLCameraUpdateSystem.h>
#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/OpenGLRenderContext.h>
#include <OpenGL/Render/OpenGLRenderStorage.h>

#include <GLFW/glfw3.h>

using namespace Mani;

OpenGLRenderContext createContext(ECS::Registry& registry)
{
	OpenGLRenderContext context;

	{
		// camera
		ECS::EntityId cameraId = CameraStatics::getMainCameraId(registry);
		MANI_ASSERT(cameraId != ECS::INVALID_ID, "trying to render without a camera");
		auto position = registry.get<Position>(cameraId);
		auto camera = registry.get<Camera>(cameraId);

		context.cameraPosition = *position;

		context.view = camera->view;
		context.projection = camera->projection;
		context.width = static_cast<int>(camera->width);
		context.height = static_cast<int>(camera->height);
	}

	{
		// light
		for (const auto [entityId, light] : ECS::ConstView<DirectionalLight>(registry))
		{
			context.directionalLights.add(light);
		}
		for (const auto [entityId, light, position] : ECS::ConstView<PointLight, Position>(registry))
		{
			context.pointLights.add(std::tuple<PointLight, Position>{ light, position });
		}
		for (const auto [entityId, light, position, rotation] : ECS::ConstView<Spotlight, Position, Rotation>(registry))
		{
			context.spotlights.add(std::tuple<Spotlight, Position, Rotation>{ light, position, rotation });
		}
	}

	{
		// clear color
		Ref<OpenGLClearColor> clearColor = registry.getSingle<OpenGLClearColor>();
		context.clearColor = clearColor->color;
	}

	{
		// renderers and extensions
		OpenGLRenderStorage& storage = registry.getSinglePinned<OpenGLRenderStorage>();
		context.renderers = storage.renderers;
		context.extensions = storage.extensions;

		Ref<RenderContext> renderContext = registry.getSingle<RenderContext>();
		context.renderContext = *renderContext;
	}

	return context;
}

void OpenGLRenderSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();
	world.initializeDependency<CameraSystem>();
	world.initializeDependency<OpenGLCameraUpdateSystem>();
	world.initializeDependency<RenderContextSystem>();
	
	registry.addSingle<OpenGLClearColor>();
	OpenGLRenderStorage& storage = registry.addSinglePinned<OpenGLRenderStorage>();
	storage.renderThread = &m_renderThread;
}

void OpenGLRenderSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	OpenGLRenderStorage& storage = registry.getSinglePinned<OpenGLRenderStorage>();
	storage.renderThread = nullptr;
	
	m_renderThread.stop();

	registry.removeSingle<OpenGLClearColor>();
	registry.removeSinglePinned<OpenGLRenderStorage>();
}

void OpenGLRenderSystem::tick(ECS::Registry& registry)
{
	OpenGLCommandBuffers* cbs = registry.findSinglePinned<OpenGLCommandBuffers>();
	if (cbs == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Trying to push opengl commands without a command buffer");
		return;
	}

	if (!cbs->isReadBufferValid())
	{
		return;
	}

	OpenGLRenderContext context = createContext(registry);
	OpenGLCommandBuffer& buffer = cbs->buffers[cbs->readBuffer];

#if MANI_DEBUG // display fps in window title
	OpenGLWindowContext& openglContext = registry.getSinglePinned<OpenGLWindowContext>();
	Ref<Time> time = registry.getSingle<Time>();
	const int fps = Math::isEqual(time->delta, 0.f) ? 0 : static_cast<int>(1 / time->delta);
	glfwSetWindowTitle(openglContext.window, std::format("{} ({}fps)", openglContext.name, fps).c_str());
#endif

	if (context.renderers.isEmpty())
	{
		MANI_LOG_WARNING(LogOpenGL, "No renderer registered");
		MANI_ASSERT(buffer.isReadyToWrite != nullptr, "Invalid buffer");
		buffer.isReadyToWrite->release(); // skip this frame
		return;
	}

	m_renderThread.enqueue([&registry, &buffer, context = std::move(context)]() mutable
	{
		OpenGLWindowContext& openglContext = registry.getSinglePinned<OpenGLWindowContext>();
		MANI_TIME_SCOPE("OpenGLRenderSystem_tick_renderthread");
		glfwMakeContextCurrent(openglContext.window);

		// setting color state.
		const Vec4f& clearColor = context.clearColor;
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

		// consuming color state.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, context.width, context.height);

		// command list should be sorted by renderer id.
		// This is done so we can layer the renderers and go through command in order
		auto commandIt = buffer.commands.begin();
		size_t commandIndex = 0;
		for (IOpenGLRenderer* renderer : context.renderers)
		{
			MANI_ASSERT(renderer != nullptr, "null renderer");
			
			renderer->onBegin(context);
			
			for (; commandIndex < buffer.commands.count(); commandIndex++)
			{
				OpenGLCommand& command = buffer.commands[commandIndex];
				if (command.rendererId > renderer->getId())
				{
					break; // get to the corresponding renderer for the current command
				}
				else if (command.rendererId < renderer->getId())
				{
					MANI_LOG_WARNING(LogOpenGL, "No renderer exist for command with rendererId {}, commands are being skipped", command.rendererId);
					continue;
				}

				// render
				renderer->render(command, context);
			}

			renderer->onEnd(context);
		}

#if MANI_DEBUG
		if (commandIndex < buffer.commands.count())
		{
			MANI_LOG_WARNING(LogOpenGL, "No renderer exist for command with rendererId {}, commands are being skipped", buffer.commands[commandIndex].rendererId);
		}
#endif

		// render extension if any
		for (auto* extension : context.extensions)
		{
			extension->onPostRender(registry);
		}

		glfwSwapBuffers(openglContext.window);
		glfwMakeContextCurrent(nullptr);

		MANI_ASSERT(buffer.isReadyToWrite != nullptr, "Invalid buffer");
		buffer.isReadyToWrite->release();
	});
}
