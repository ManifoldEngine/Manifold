#include "OpenGLRenderSystem.h"

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
void loadQuad(uint32_t repeatAmount, Resource<OpenGLVertexArray>& res);

OpenGLRenderSystem::Storage* getStorageChecked(ECS::Registry& registry)
{
	OpenGLRenderSystem::Storage* storage = registry.getSingle<OpenGLRenderSystem::Storage>();
	MANI_ASSERT(storage != nullptr, "trying to register an extension outside of the lifetime of the system");
	return storage;
}

void OpenGLRenderSystem::onInitialize(ECS::Registry& registry, World& world)
{
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

void OpenGLRenderSystem::tick(float deltaTime, ECS::Registry& registry)
{
	MANI_TIME_SCOPE(OpenGLRenderSystemtick);

	OpenGLCommandBuffer3D* cbs = registry.getSingle<OpenGLCommandBuffer3D>();
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
	context.readBuffer = cbs->readBuffer;

#if MANI_DEBUG
	const int fps = Math::isEqual(deltaTime, 0.f) ? 0 : static_cast<int>(1 / deltaTime);
	glfwSetWindowTitle(context.openglContext->window, std::format("{} ({}fps)", context.openglContext->name, fps).c_str());
#endif

	storage.renderThread.enqueue([&registry, cbs, context = std::move(context)]() mutable
	{
		MANI_TIME_SCOPE(OpenGLRenderSystemtickrenderthread);
		glfwMakeContextCurrent(context.openglContext->window);

		const std::vector<OpenGLCommand>& commands = cbs->buffers[context.readBuffer];
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
			for (; commandId < commands.size(); commandId++)
			{
				const OpenGLCommand& command = commands[commandId];
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

		cbs->renderFrame++;
		glfwSwapBuffers(context.openglContext->window);
		glfwMakeContextCurrent(nullptr);
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
		ECS::View<Position, Camera> cameraView(registry);
		auto it = cameraView.begin();
		MANI_ASSERT(it != cameraView.end(), "Trying to render without a camera");

		const Position& position = *registry.get<Position>(*it);
		context.cameraPosition = position.value;

		const Camera& camera = *registry.get<Camera>(*it);
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
			context.directionalLights.emplace_back(light);
		}
		for (const auto entityId : ECS::View<PointLight, Position> (registry))
		{
			const PointLight& light = *registry.get<PointLight>(entityId);
			const Position& position = *registry.get<Position>(entityId);
			context.pointLights.emplace_back(std::tuple<PointLight, Position>{ light, position });
		}
		for (const auto entityId : ECS::View<Spotlight, Position, Rotation> (registry))
		{
			const Spotlight& light = *registry.get<Spotlight>(entityId);
			const Position& position = *registry.get<Position>(entityId);
			const Rotation& rotation = *registry.get<Rotation>(entityId);
			context.spotlights.emplace_back(std::tuple<Spotlight, Position, Rotation>{ light, position, rotation });
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

void loadQuad(uint32_t repeatAmount, Resource<OpenGLVertexArray>& res)
{
	
}

template<typename T>
void push_back_if_unique(std::vector<T*>& vector, T* element)
{
	auto it = std::find(vector.begin(), vector.end(), element);
	if (it == vector.end())
	{
		vector.push_back(element);
	}
}

void OpenGLRenderSystem::registerExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "Cannot register a null extension");
	OpenGLRenderSystem::Storage* storage = getStorageChecked(registry);
	push_back_if_unique(storage->extensions, extension);
}

void OpenGLRenderSystem::unregisterExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	if (OpenGLRenderSystem::Storage* storage = registry.getSingle<OpenGLRenderSystem::Storage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		std::vector<IOpenGLRenderExtension*>& extensions = storage->extensions;
		if (auto it = std::find(extensions.begin(), extensions.end(), extension); it != extensions.end())
		{
			extensions.erase(it);
		}
	}
}

void OpenGLRenderSystem::registerRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	MANI_ASSERT(renderer != nullptr, "Cannot register a null renderer");

	OpenGLRenderSystem::Storage* storage = getStorageChecked(registry);
	std::vector<IOpenGLRenderer*>& renderers = storage->renderers;
	push_back_if_unique(renderers, renderer);

	std::sort(renderers.begin(), renderers.end(), [](const IOpenGLRenderer* lhs, const IOpenGLRenderer* rhs)
	{
		return lhs->getId() < rhs->getId();
	});
}

void OpenGLRenderSystem::unregisterRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	if (OpenGLRenderSystem::Storage* storage = registry.getSingle<OpenGLRenderSystem::Storage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		std::vector<IOpenGLRenderer*>& renderers = storage->renderers;
		if (auto it = std::find(renderers.begin(), renderers.end(), renderer); it != renderers.end())
		{
			renderers.erase(it);
		}
	}
}
