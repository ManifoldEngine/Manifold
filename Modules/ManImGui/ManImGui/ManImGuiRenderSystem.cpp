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
#include <ManImGui/Components/ManImGuiWindowContext.h>

using namespace Mani;

struct ManImGuiRenderSystem::Storage
{
	ImDrawData* drawData = nullptr;
};

void ManImGuiRenderSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<OpenGLRenderSystem>();

	registry.addSinglePinned<ManImGuiRenderSystem::Storage>();

	OpenGL::registerExtension(registry, &extension);
	extension.isDrawDataBeingRead.release();
}

void ManImGuiRenderSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	OpenGL::unregisterExtension(registry, &extension);
	registry.removeSinglePinned<ManImGuiRenderSystem::Storage>();
}

void Mani::ManImGuiRenderSystem::tick(ECS::Registry& registry)
{
	ManImGuiRenderSystem::Storage& storage = registry.getSinglePinned<ManImGuiRenderSystem::Storage>();
	Ref<ManImGuiWindowContext> context = registry.getSingle<ManImGuiWindowContext>();

	switch (context->mode)
	{
		case EManImGuiMode::Hidden: break;	
		case EManImGuiMode::Show:
		{
			extension.isDrawDataBeingRead.acquire();
			MANI_ASSERT(storage.drawData == nullptr, "Draw data should have been consumed by that point.");
			ImGui::Render();
			storage.drawData = ImGui::GetDrawData();
			break;
		}
	}
}

void ManImGuiRenderSystemExtension::onPostRender(ECS::Registry& registry) const
{
	MANI_TIME_SCOPE("ManImGuiRenderSystemExtension_onPostRender");
	ManImGuiRenderSystem::Storage& storage = registry.getSinglePinned<ManImGuiRenderSystem::Storage>();
	if (storage.drawData != nullptr)
	{
		ImGui_ImplOpenGL3_RenderDrawData(storage.drawData);
		storage.drawData = nullptr;
		isDrawDataBeingRead.release();
	}
}
