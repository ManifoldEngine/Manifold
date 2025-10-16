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
	return ManImGuiStatics::isShowing(registry);
}

void ManImGuiTopMenuBarSystem::tick(ECS::Registry& registry)
{
	if (ImGui::BeginMainMenuBar())
	{
		ECS::View<ManImGuiMenu> menuView(registry);
		for (const auto entityId : menuView)
		{
			ManImGuiMenu& menu = registry.getRef<ManImGuiMenu>(entityId);
			displayMenu(menu.subMenu, menu.title);
		}

		ImGui::EndMainMenuBar();
	}
}
