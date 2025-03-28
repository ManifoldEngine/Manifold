#include "ManImGuiMainLoopSystem.h"

#include "IsManImGuiDisplayed.h"

#include <Inputs/Data/InputDevice.h>
#include <Inputs/Data/InputUser.h>
#include <Inputs/InputSystem.h>
#include <Inputs/Cursor.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace Mani;

const std::string TOGGLE_MANIMGUI = "TOGGLE_MANIMGUI";

struct CursorModeCache
{
	Cursor::EMode mode = Cursor::EMode::DISABLED;
};

void Mani::ManImGuiMainLoopSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
	systemContainer.initializeDependency<InputSystem>();

	{
		const ECS::EntityId entityId = registry.create();

		registry.add<IsManImGuiDisplayed>(entityId);

		InputUser& inputUser = *registry.add<InputUser>(entityId);
		inputUser.setAction(TOGGLE_MANIMGUI);
		inputUser.addBinding("F7", TOGGLE_MANIMGUI);

		for (const auto deviceId : ECS::View<InputDevice>(registry))
		{
			inputUser.inputDevices.push_back(deviceId);
		}
	}

	registry.addSingle<CursorModeCache>();
}

void Mani::ManImGuiMainLoopSystem::onDeinitialize(ECS::Registry& registry)
{
	for (const auto entityId : ECS::View<IsManImGuiDisplayed>(registry))
	{
		registry.deferDestroy(entityId);
	}

	registry.removeSingle<CursorModeCache>();
}

void Mani::ManImGuiMainLoopSystem::tick(float deltaTime, Mani::ECS::Registry& registry)
{
	const ECS::View<IsManImGuiDisplayed, InputUser> view(registry);
	const ECS::EntityId entityId = view.first();
	
	IsManImGuiDisplayed& isManImGuiDisplayed = *registry.get<IsManImGuiDisplayed>(entityId);
	const InputUser& inputUser = *registry.get<InputUser>(entityId);
	const InputAction& toggleManImGui = inputUser.actions.at(TOGGLE_MANIMGUI);

	handleInputs(registry, isManImGuiDisplayed, toggleManImGui);
	
	if (!isManImGuiDisplayed.value)
	{
		return;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
}

void ManImGuiMainLoopSystem::handleInputs(ECS::Registry& registry, IsManImGuiDisplayed& isManImGuiDisplayed, const InputAction& toggleManImGui)
{
	if (!toggleManImGui.changed() || !toggleManImGui.isPressed)
	{
		return;
	}

	isManImGuiDisplayed.value = !isManImGuiDisplayed.value;
	
	const bool isDisplayed = isManImGuiDisplayed.value;
	Cursor& cursor = *registry.getSingle<Cursor>();
	CursorModeCache& cache = *registry.getSingle<CursorModeCache>();

	if (isDisplayed)
	{
		cache.mode = cursor.mode;
		cursor.mode = Cursor::EMode::NORMAL;
	}
	else
	{
		cursor.mode = cache.mode;
	}
}