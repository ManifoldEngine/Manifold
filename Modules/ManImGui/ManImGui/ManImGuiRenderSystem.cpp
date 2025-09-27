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
	std::binary_semaphore isDrawDataBeingRead{ 0 };
};

void ManImGuiRenderSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<OpenGLRenderSystem>();

	ManImGuiRenderSystem::Storage& storage = *registry.addSingle<ManImGuiRenderSystem::Storage>();
	storage.isDrawDataBeingRead.release();

	OpenGLRenderSystem::registerExtension(registry, &extension);
}

void ManImGuiRenderSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	OpenGLRenderSystem::unregisterExtension(registry, &extension);
	registry.removeSingle<ManImGuiRenderSystem::Storage>();
}

void Mani::ManImGuiRenderSystem::tick(ECS::Registry& registry)
{
	ManImGuiRenderSystem::Storage& storage = *registry.getSingle<ManImGuiRenderSystem::Storage>();
	ManImGuiWindowContext& context = *registry.getSingle<ManImGuiWindowContext>();

	switch (context.mode)
	{
		case EManImGuiMode::Hidden: break;	
		case EManImGuiMode::Show:
		{
			storage.isDrawDataBeingRead.acquire();
			MANI_ASSERT(storage.drawData == nullptr, "Draw data should have been consumed by that point.");
			ImGui::Render();
			storage.drawData = ImGui::GetDrawData();
			break;
		}
	}
}

void ManImGuiRenderSystemExtension::onPostRender(ECS::Registry& registry) const
{
	MANI_TIME_SCOPE(ManImGuiRenderSystemExtension_onPostRender);
	ManImGuiRenderSystem::Storage& storage = *registry.getSingle<ManImGuiRenderSystem::Storage>();
	if (storage.drawData != nullptr)
	{
		ImGui_ImplOpenGL3_RenderDrawData(storage.drawData);
		storage.drawData = nullptr;
		storage.isDrawDataBeingRead.release();
	}
}
