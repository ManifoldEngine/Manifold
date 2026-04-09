#include "ManImGuiInputDebugSystem.h"

#include <Core/CoreFwd.h>

#include <Inputs/InputSystem.h>
#include <Inputs/Components/InputUser.h>
#include <Inputs/Components/InputDevice.h>

#include <ManImGui/ManImGui.h>
#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiManifoldMenuSystem.h>
#include <ManImGui/Components/ManImGuiWindowContext.h>

#include "imgui.h"

using namespace Mani;

constexpr std::string_view INPUTS_NAME = "Inputs";

std::string_view getControlName(const ECS::Registry& registry, const InputUser& inputUser, ControlId controlId)
{
#if MANI_DEBUG
	for (const auto deviceId : inputUser.inputDevices)
	{
		Ref<const InputDevice> device = registry.get<InputDevice>(deviceId);

		for (const auto& [hint, id] : device->buttonHints)
		{
			if (id != controlId)
			{
				continue;
			}

			if (const std::string_view* str = device->debug_hintTobuttonNames.find(hint))
			{
				return *str;
			}
		}

		for (const auto& axis : device->axis)
		{
			if (axis.id == controlId)
			{
				return axis.debug_name;
			}
		}
	}
#endif
	return "";
}

void drawInputDevice(const InputDevice& device)
{
	ImGui::Text(device.deviceName.c_str());
	{
		constexpr int columnsCount = 4;
		constexpr ImVec2 outerSize = { 0.f, 0.f };
		constexpr float innerWidth = 0.f;
		constexpr ImGuiTableFlags flags = ImGuiTableFlags_::ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_::ImGuiTableFlags_Borders;
		if (ImGui::BeginTable(device.deviceName.c_str(), columnsCount, flags, outerSize, innerWidth))
		{
			ImGui::TableSetupColumn("Axis");
			ImGui::TableSetupColumn("x");
			ImGui::TableSetupColumn("y");
			ImGui::TableSetupColumn("z");
			ImGui::TableHeadersRow();

			for (const auto& axis : device.axis)
			{
				ImGui::TableNextRow();
#if MANI_DEBUG
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(axis.debug_name.data());
#endif
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.2f", axis.x);

				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%.2f", axis.y);

				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%.2f", axis.z);
			}
			ImGui::EndTable();
		}
	}
	{
		ImGui::Text("Button buffer");
		ImGui::SameLine();
		ImGui::Separator();
		ImGui::SameLine();
		for (auto& button : device.buttonBuffer)
		{
			if (button.isPressed)
			{
#if MANI_DEBUG
				ImGui::Text(std::format("{} ", button.debug_name).c_str());
				ImGui::SameLine();
#endif
			}
		}
	}
}

void drawInputActionRow(const InputAction& action, const std::string_view& controlName)
{
	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(std::format("[{}]", controlName).c_str());

	ImGui::TableSetColumnIndex(1);
	ImGui::Text(std::format("[{}]", action.name).c_str());
	
	auto pickColor = [](bool value)
	{
		constexpr ImVec4 active{ 1.f, 1.f, 1.f, 1.f };
		constexpr ImVec4 inactive{ 0.25f, 0.25f, 0.25f, 1.f };
		return value ? active : inactive;
	};

	ImGui::TableSetColumnIndex(2);
	ImGui::TextColored(pickColor(action.isEnabled), "enabled");

	ImGui::TableSetColumnIndex(3);
	ImGui::Text(action.xyz().toString().c_str());

	ImGui::TableSetColumnIndex(4);
	ImGui::TextColored(pickColor(action.isPressed), "pressed");
}

void drawInputUser(const ECS::Registry& registry, const InputUser& inputUser, const ECS::EntityId id)
{
	const std::string title = std::format("InputUser[{}]", id);
	ImGui::Text(title.c_str());

	constexpr int columnsCount = 5;
	constexpr ImVec2 outerSize = { 0.f, 0.f };
	constexpr float innerWidth = 0.f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_::ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_::ImGuiTableFlags_Borders;
	if (ImGui::BeginTable(title.c_str(), columnsCount, flags, outerSize, innerWidth))
	{
		for (const auto& [controlId, actionIds] : inputUser.bindings)
		{
			const std::string_view controlName = getControlName(registry, inputUser, controlId);
			for (const auto& actionId : actionIds)
			{
				const InputAction& action = inputUser.actions[actionId];
				drawInputActionRow(action, controlName);
			}
		}

		for (const auto& [controlId, bindings] : inputUser.buttonToAxisBindings)
		{
			const std::string_view controlName = getControlName(registry, inputUser, controlId);
			for (const auto& axisActionBinding : bindings)
			{
				const InputAction& action = inputUser.actions[axisActionBinding.actionId];
				drawInputActionRow(action, controlName);
			}
		}
		ImGui::EndTable();
	}
}

void ManImGuiInputDebugSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<ManImGuiManifoldMenuSystem>();

	{
		Ref<ManImGuiMenu> menu = ManImGuiStatics::Manifold::getMenu(registry);
		menu->subMenu.addItem(INPUTS_NAME);
	}
}

bool Mani::ManImGuiInputDebugSystem::shouldTick(const ECS::Registry& registry) const
{
	if (!ManImGuiStatics::isShowing(registry))
	{
		return false;
	}

	Ref<const ManImGuiMenu> menu = ManImGuiStatics::Manifold::getMenu(registry);
	return menu->subMenu.getSelected(INPUTS_NAME);
}

void ManImGuiInputDebugSystem::tick(ECS::Registry& registry)
{
	bool isOpened = true;
	if (!ImGui::Begin("Input Debug", &isOpened, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (!isOpened)
	{
		Ref<ManImGuiMenu> menu = ManImGuiStatics::Manifold::getMenu(registry);
		menu->subMenu.setSelected(INPUTS_NAME, false);
		ImGui::End();
		return;
	}

	{
		// Devices
		constexpr ImGuiChildFlags flags = ImGuiChildFlags_::ImGuiChildFlags_Border | ImGuiChildFlags_::ImGuiChildFlags_AutoResizeY;
		ImGui::BeginChild("Devices", ImVec2{ 0.f, 0.f }, flags);
		for (const auto [entityId, device] : ECS::ConstView<InputDevice>(registry))
		{
			ImGui::Separator();
			drawInputDevice(device);
		}
		ImGui::EndChild();
	}

	{
		// Input Actions
		constexpr ImGuiChildFlags flags = ImGuiChildFlags_::ImGuiChildFlags_Border | ImGuiChildFlags_::ImGuiChildFlags_AutoResizeY;
		ImGui::BeginChild("Inputs Users", ImVec2{ 0.f, 0.f }, flags);
		for (const auto [entityId, user] : ECS::ConstView<InputUser>(registry))
		{
			drawInputUser(registry, user, entityId);
		}
		ImGui::EndChild();
	}

	ImGui::End();
}