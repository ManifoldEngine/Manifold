#include "ManImGuiTopMenuBarSystem.h"

#include <Inputs/Inputs.h>
#include <Inputs/InputSystem.h>

#include <ManImGui/ManImGui.h>
#include <ManImGui/Components/ManImGuiMenu.h>

#include <imgui.h>

using namespace Mani;

bool displayMenu(ManImGuiSubMenu& menu, const std::string_view& title)
{
	if (menu.items.isEmpty())
	{
		return false;
	}

	if (ImGui::BeginMenu(title.data()))
	{
		for (auto& [name, item] : menu.items)
		{
			if (!displayMenu(item.subMenu, name))
			{
				ImGui::MenuItem(name.data(), nullptr, &item.selected);
			}
		}

		ImGui::EndMenu();
	}

	return true;
}

bool ManImGuiTopMenuBarSystem::shouldTick(const ECS::Registry& registry) const
{
	return ManImGui::isShowing(registry);
}

void ManImGuiTopMenuBarSystem::tick(ECS::Registry& registry)
{
	if (ImGui::BeginMainMenuBar())
	{
		for (auto [entityId, menu] : ECS::View<ManImGuiMenu>(registry))
		{
			displayMenu(menu.subMenu, menu.title);
		}

		ImGui::EndMainMenuBar();
	}
}
