#include "ManImGuiMainLoopSystem.h"

#include <Inputs/Data/InputUser.h>
#include <Inputs/InputSystem.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace Mani;

constexpr std::string_view TOGGLE_MANIMGUI = "ToggleManImGui"

struct ManImGuiIsDisplayed
{
	bool isDisplayed = false;
};

void Mani::ManImGuiMainLoopSystem::tick(float deltaTime, Mani::ECS::Registry& registry)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Mani::ManImGuiMainLoopSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
	systemContainer.initializeDependency<InputSystem>();
	
	{
		const ECS::EntityId entityId = registry.create();
		
		registry.add<ManImGuiIsDisplayed>();
		
		InputUser& inputUser = *registry.add<InputUser>(entityId);
		InputUtils::addBinding(inputUser, "F7", TOGGLE_MANIMGUI);
	}
}

void Mani::ManImGuiMainLoopSystem::onDeinitialize(ECS::Registry& registry)
{
	auto view = ECS::View<ManImGuiIsDisplayed>(registry);
	for (view)
}
