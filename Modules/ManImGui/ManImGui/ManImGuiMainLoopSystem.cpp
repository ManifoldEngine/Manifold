#include "ManImGuiMainLoopSystem.h"

#include "IsManImGuiDisplayed.h"

#include <Inputs/Data/InputDevice.h>
#include <Inputs/Data/InputUser.h>
#include <Inputs/InputSystem.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace Mani;

const std::string TOGGLE_MANIMGUI = "TOGGLE_MANIMGUI";

void Mani::ManImGuiMainLoopSystem::tick(float deltaTime, Mani::ECS::Registry& registry)
{
	const ECS::View<IsManImGuiDisplayed, InputUser> view(registry);
	const ECS::EntityId entityId = view.first();
	
	IsManImGuiDisplayed& isManImGuiDisplayed = *registry.get<IsManImGuiDisplayed>(entityId);
	InputUser& inputUser = *registry.get<InputUser>(entityId);
	InputAction& toggleManImGui = inputUser.actions[TOGGLE_MANIMGUI];
	if (toggleManImGui.changed() && toggleManImGui.isPressed)
	{
		isManImGuiDisplayed.value = !isManImGuiDisplayed.value;
	}

	if (!isManImGuiDisplayed.value)
	{
		return;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
}

void Mani::ManImGuiMainLoopSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
	systemContainer.initializeDependency<InputSystem>();
	
	{
		const ECS::EntityId entityId = registry.create();

		registry.add<IsManImGuiDisplayed>(entityId);

		InputUser& inputUser = *registry.add<InputUser>(entityId);
		InputUtils::setAction(inputUser, TOGGLE_MANIMGUI);
		InputUtils::addBinding(inputUser, "F7", TOGGLE_MANIMGUI);

		for (const auto deviceId : ECS::View<InputDevice>(registry))
		{
			inputUser.inputDevices.push_back(deviceId);
		}
	}
}

void Mani::ManImGuiMainLoopSystem::onDeinitialize(ECS::Registry& registry)
{
	for (const auto entityId : ECS::View<IsManImGuiDisplayed>(registry))
	{
		registry.deferDestroy(entityId);
	}
}
