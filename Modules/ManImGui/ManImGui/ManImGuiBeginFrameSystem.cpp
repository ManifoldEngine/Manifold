#include "ManImGuiBeginFrameSystem.h"

#include <Inputs/Data/InputDevice.h>
#include <Inputs/Data/InputUser.h>
#include <Inputs/InputSystem.h>
#include <Inputs/Cursor.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiWindowContext.h>

#include <GLFW/glfw3.h>

using namespace Mani;

const std::string TOGGLE_MANIMGUI = "TOGGLE_MANIMGUI";

struct ManImGuiUser
{
	Cursor::EMode modeCache = Cursor::EMode::Disabled;
};

void handleInputs(ECS::Registry& registry, ManImGuiWindowContext& context, ManImGuiUser& manImguiUser, const InputAction& toggleManImGui)
{
	if (!toggleManImGui.changed() || !toggleManImGui.isPressed)
	{
		return;
	}

	Cursor& cursor = *registry.getSingle<Cursor>();

	switch (context.mode)
	{
		case EManImGuiMode::Show: 
		{
			context.mode = EManImGuiMode::Hidden;
			cursor.mode = manImguiUser.modeCache;
			break;
		}

		case EManImGuiMode::Hidden: 
		{
			context.mode = EManImGuiMode::Show; 
			manImguiUser.modeCache = cursor.mode;
			cursor.mode = Cursor::EMode::Normal;
			break;
		}
		default: break;
	}
}

void Mani::ManImGuiBeginFrameSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<InputSystem>();

	{
		const ECS::EntityId entityId = registry.create();

		registry.add<ManImGuiUser>(entityId);

		InputUser& inputUser = *registry.add<InputUser>(entityId);
		inputUser.setAction(TOGGLE_MANIMGUI);
		inputUser.addBinding("F7", TOGGLE_MANIMGUI);

		for (const auto deviceId : ECS::View<InputDevice>(registry))
		{
			inputUser.inputDevices.push_back(deviceId);
		}
	}
}

void Mani::ManImGuiBeginFrameSystem::onDeinitialize(ECS::Registry& registry)
{
	for (const auto entityId : ECS::View<ManImGuiUser, InputUser>(registry))
	{
		registry.deferDestroy(entityId);
	}
}

void Mani::ManImGuiBeginFrameSystem::tick(float deltaTime, Mani::ECS::Registry& registry)
{
	ManImGuiWindowContext& context = *registry.getSingle<ManImGuiWindowContext>();
	for (const auto entityId : ECS::View<ManImGuiUser, InputUser>(registry))
	{
		auto [imguiUser, inputUser] = registry.getMany<ManImGuiUser, InputUser>(entityId);
		const InputAction& toggleManImGui = inputUser->actions.at(TOGGLE_MANIMGUI);
		handleInputs(registry, context, *imguiUser, toggleManImGui);
		break;
	}
	
	if (context.mode == EManImGuiMode::Hidden)
	{
		return;
	}

	glfwMakeContextCurrent(context.window);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	glfwMakeContextCurrent(nullptr);
}
