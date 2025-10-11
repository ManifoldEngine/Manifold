#include "ManImGuiInputDebugSystem.h"

#include <Core/CoreFwd.h>

#include <Inputs/InputSystem.h>
#include <Inputs/Data/InputUser.h>
#include <Inputs/Data/InputDevice.h>

#include <ManImGui/ManImGui.h>
#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiManifoldMenuSystem.h>
#include <ManImGui/Components/ManImGuiWindowContext.h>

#include "imgui.h"

using namespace Mani;

constexpr std::string_view INPUTS_NAME = "Inputs";

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

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(axis.name.c_str());

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
				ImGui::Text(std::format("{} ", button.name).c_str());
				ImGui::SameLine();
			}
		}
	}
}

void drawInputActionRow(InputAction& action, const std::string& binding)
{
	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(std::format("[{}]", binding).c_str());

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

void drawInputUser(InputUser& inputUser, const ECS::EntityId id)
{
	const std::string title = std::format("InputUser[{}]", id);
	ImGui::Text(title.c_str());

	constexpr int columnsCount = 5;
	constexpr ImVec2 outerSize = { 0.f, 0.f };
	constexpr float innerWidth = 0.f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_::ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_::ImGuiTableFlags_Borders;
	if (ImGui::BeginTable(title.c_str(), columnsCount, flags, outerSize, innerWidth))
	{
		for (const auto& [binding, actionNames] : inputUser.bindings)
		{
			for (const auto& actionName : actionNames)
			{
				InputAction& action = inputUser.actions[actionName];
				drawInputActionRow(action, binding);
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
		const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
		ManImGuiStatics::Menu::addItem(registry, debugMenuId, INPUTS_NAME);
	}
}

bool Mani::ManImGuiInputDebugSystem::shouldTick(const ECS::Registry& registry) const
{
	return ManImGuiStatics::isShowing(registry);
}

void ManImGuiInputDebugSystem::tick(ECS::Registry& registry)
{
	const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
	if (!ManImGuiStatics::Menu::isOpened(registry, debugMenuId, INPUTS_NAME))
	{
		return;
	}

	bool isOpened = true;
	if (!ImGui::Begin("Input Debug", &isOpened, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (!isOpened)
	{
		ManImGuiStatics::Menu::close(registry, debugMenuId, INPUTS_NAME);
		ImGui::End();
		return;
	}

	{
		// Devices
		constexpr ImGuiChildFlags flags = ImGuiChildFlags_::ImGuiChildFlags_Border | ImGuiChildFlags_::ImGuiChildFlags_AutoResizeY;
		ImGui::BeginChild("Devices", ImVec2{ 0.f, 0.f }, flags);
		for (const auto entityId : ECS::View<InputDevice>(registry))
		{
			ImGui::Separator();
			InputDevice& device = *registry.get<InputDevice>(entityId);
			drawInputDevice(device);
		}
		ImGui::EndChild();
	}

	{
		// Input Actions
		constexpr ImGuiChildFlags flags = ImGuiChildFlags_::ImGuiChildFlags_Border | ImGuiChildFlags_::ImGuiChildFlags_AutoResizeY;
		ImGui::BeginChild("Inputs Users", ImVec2{ 0.f, 0.f }, flags);
		for (const auto entityId : ECS::View<InputUser>(registry))
		{
			InputUser& inputUser = *registry.get<InputUser>(entityId);
			drawInputUser(inputUser, entityId);
		}
		ImGui::EndChild();
	}

	ImGui::End();
}