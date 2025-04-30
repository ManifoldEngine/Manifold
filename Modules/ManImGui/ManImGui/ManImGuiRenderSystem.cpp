#include "ManImGuiRenderSystem.h"

#include <Core/Debug/Profiling.h>

#include <OpenGL/OpenGLWindowContext.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>
#include <OpenGL/Render/OpenGLCommand.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiWindowContext.h>

using namespace Mani;

struct ManImGuiRenderSystem::Storage
{
	ImDrawData* drawData = nullptr;
	unsigned long long frame = 0;
	std::atomic<unsigned long long> renderFrame = 0;
};

void ManImGuiRenderSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<OpenGLRenderSystem>();

	registry.addSingle<ManImGuiRenderSystem::Storage>();
	OpenGLRenderSystem::registerExtension(registry, &extension);
}

void ManImGuiRenderSystem::onDeinitialize(ECS::Registry& registry)
{
	OpenGLRenderSystem::unregisterExtension(registry, &extension);
	registry.removeSingle<ManImGuiRenderSystem::Storage>();
}

void Mani::ManImGuiRenderSystem::tick(float deltaTime, ECS::Registry& registry)
{
	ManImGuiRenderSystem::Storage& storage = *registry.getSingle<ManImGuiRenderSystem::Storage>();
	ManImGuiWindowContext& context = *registry.getSingle<ManImGuiWindowContext>();

	switch (context.mode)
	{
		case EManImGuiMode::Hidden:
		{
			storage.frame++;
			storage.renderFrame = storage.frame;
			storage.drawData = nullptr;
			break;
		}
		
		case EManImGuiMode::Show:
		{
			while (storage.frame != storage.renderFrame)
			{
				std::this_thread::yield();
			}

			ImGui::Render();
			storage.drawData = ImGui::GetDrawData();
			storage.frame++;
			break;
		}
	}
}

void ManImGuiRenderSystemExtension::onPostRender(ECS::Registry& registry) const
{
	MANI_TIME_SCOPE(ManImGuiRenderSystemExtensiononPostRender);
	ManImGuiRenderSystem::Storage& storage = *registry.getSingle<ManImGuiRenderSystem::Storage>();
	if (storage.renderFrame < storage.frame)
	{
		ImGui_ImplOpenGL3_RenderDrawData(storage.drawData);
		storage.renderFrame++;
	}
}
