#include "OpenGLRenderSystem.h"

#include <Core/TimeSystem.h>
#include <Core/Debug/Profiling.h>
#include <Core/Async/ThreadPool.h>

#include <Camera/CameraSystem.h>

#include <Resources/Resource.h>
#include <Resources/ResourceSystem.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/OpenGLWindowContext.h>

#include <OpenGL/Data/OpenGLClearColor.h>

#include <OpenGL/Render/IOpenGLRenderExtension.h>
#include <OpenGL/Render/IOpenGLRenderer.h>
#include <OpenGL/Render/OpenGLCameraUpdateSystem.h>
#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/OpenGLRenderContext.h>

#include <GLFW/glfw3.h>

using namespace Mani;

OpenGLRenderContext createContext(ECS::Registry& registry);

OpenGLRenderSystem::Storage* getStorageChecked(ECS::Registry& registry)
{
	OpenGLRenderSystem::Storage* storage = registry.getSingle<OpenGLRenderSystem::Storage>();
	MANI_ASSERT(storage != nullptr, "trying to register an extension outside of the lifetime of the system");
	return storage;
}

void OpenGLRenderSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();
	world.initializeDependency<CameraSystem>();
	world.initializeDependency<OpenGLCameraUpdateSystem>();
	
	registry.addSingle<OpenGLRenderSystem::Storage>();
	registry.addSingle<OpenGLClearColor>();
}

void OpenGLRenderSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	OpenGLRenderSystem::Storage& storage = *registry.getSingle<OpenGLRenderSystem::Storage>();
	storage.renderThread.stop();

	registry.removeSingle<OpenGLClearColor>();
	registry.removeSingle<OpenGLRenderSystem::Storage>();
}

void OpenGLRenderSystem::tick(ECS::Registry& registry)
{
	MANI_TIME_SCOPE(OpenGLRenderSystem_tick);

	OpenGLCommandBufferCollection* cbs = registry.getSingle<OpenGLCommandBufferCollection>();
	if (cbs == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Trying to push opengl commands without a command buffer");
		return;
	}

	if (!cbs->isReadBufferValid())
	{
		return;
	}

	OpenGLRenderSystem::Storage& storage = *registry.getSingle<OpenGLRenderSystem::Storage>();
	OpenGLRenderContext context = createContext(registry);
	OpenGLCommandBuffer& buffer = cbs->buffers[cbs->readBuffer];

#if MANI_DEBUG
	Time& time = *registry.getSingle<Time>();
	const int fps = Math::isEqual(time.delta, 0.f) ? 0 : static_cast<int>(1 / time.delta);
	glfwSetWindowTitle(context.openglContext->window, std::format("{} ({}fps)", context.openglContext->name, fps).c_str());
#endif

	storage.renderThread.enqueue([&registry, &buffer, context = std::move(context)]() mutable
	{
		MANI_TIME_SCOPE(OpenGLRenderSystem_tick_renderthread);
		glfwMakeContextCurrent(context.openglContext->window);

		glEnable(GL_DEPTH_TEST);

		// setting color state.
		const Vec4f& clearColor = context.clearColor;
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

		// consuming color state.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, context.width, context.height);

		size_t commandId = 0;
		for (auto* renderer : context.renderers)
		{
			for (; commandId < buffer.commands.count(); commandId++)
			{
				const OpenGLCommand& command = buffer.commands[commandId];
				if (renderer->getId() != command.rendererId)
				{
					break;
				}
				renderer->render(command, context);
			}
		}

		// render extension if any
		for (auto* extension : context.extensions)
		{
			extension->onPostRender(registry);
		}

		glfwSwapBuffers(context.openglContext->window);
		glfwMakeContextCurrent(nullptr);

		buffer.isReadyToWrite.release();
	});
}

OpenGLRenderContext createContext(ECS::Registry& registry)
{
	OpenGLRenderContext context;
	{
		context.openglContext = registry.getSingle<OpenGLWindowContext>();
	}

	{
		// camera
		ECS::EntityId cameraId = CameraStatics::getMainCameraId(registry);
		MANI_ASSERT(cameraId != ECS::INVALID_ID, "trying to render without a camera");
		auto [positionPtr, cameraPtr] = registry.getMany<Position, Camera>(cameraId);

		const Position& position = *positionPtr;
		context.cameraPosition = position.value;

		const Camera& camera = *cameraPtr;
		context.view = camera.view;
		context.projection = camera.projection;
		context.width = static_cast<int>(camera.width);
		context.height = static_cast<int>(camera.height);
	}

	{
		// light
		for (const auto entityId : ECS::View<DirectionalLight> (registry))
		{
			const DirectionalLight& light = *registry.get<DirectionalLight>(entityId);
			context.directionalLights.add(light);
		}
		for (const auto entityId : ECS::View<PointLight, Position> (registry))
		{
			const PointLight& light = *registry.get<PointLight>(entityId);
			const Position& position = *registry.get<Position>(entityId);
			context.pointLights.add(std::tuple<PointLight, Position>{ light, position });
		}
		for (const auto entityId : ECS::View<Spotlight, Position, Rotation> (registry))
		{
			const Spotlight& light = *registry.get<Spotlight>(entityId);
			const Position& position = *registry.get<Position>(entityId);
			const Rotation& rotation = *registry.get<Rotation>(entityId);
			context.spotlights.add(std::tuple<Spotlight, Position, Rotation>{ light, position, rotation });
		}
	}

	{
		// clear color
		const OpenGLClearColor& clearColor = *registry.getSingle<OpenGLClearColor>();
		context.clearColor = clearColor.color;
	}

	{
		OpenGLRenderSystem::Storage* storage = getStorageChecked(registry);
		context.renderers = storage->renderers;
		context.extensions = storage->extensions;
	}

	return context;
}

void OpenGLRenderSystem::registerExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "Cannot register a null extension");
	OpenGLRenderSystem::Storage* storage = getStorageChecked(registry);
	storage->extensions.addUnique(extension);
}

void OpenGLRenderSystem::unregisterExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	if (OpenGLRenderSystem::Storage* storage = registry.getSingle<OpenGLRenderSystem::Storage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		List<IOpenGLRenderExtension*>& extensions = storage->extensions;
		extensions.remove(extension);
	}
}

void OpenGLRenderSystem::registerRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	MANI_ASSERT(renderer != nullptr, "Cannot register a null renderer");

	OpenGLRenderSystem::Storage* storage = getStorageChecked(registry);
	List<IOpenGLRenderer*>& renderers = storage->renderers;
	renderers.addUnique(renderer);

	renderers.sort([](const IOpenGLRenderer* lhs, const IOpenGLRenderer* rhs)
	{
		return lhs->getId() < rhs->getId();
	});
}

void OpenGLRenderSystem::unregisterRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	if (OpenGLRenderSystem::Storage* storage = registry.getSingle<OpenGLRenderSystem::Storage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		List<IOpenGLRenderer*>& renderers = storage->renderers;
		renderers.remove(renderer);
	}
}
