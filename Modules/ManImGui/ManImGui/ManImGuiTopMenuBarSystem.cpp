#include "ManImGuiTopMenuBarSystem.h"

#include <Inputs/Inputs.h>
#include <Inputs/InputSystem.h>

#include <ManImGui/ManImGui.h>
#include <ManImGui/Components/ManImGuiMenu.h>

#include <imgui.h>

bool Mani::ManImGuiTopMenuBarSystem::shouldTick(const ECS::Registry& registry) const
{
	return ManImGuiStatics::isShowing(registry);
}

void Mani::ManImGuiTopMenuBarSystem::tick(ECS::Registry& registry)
{
	if (ImGui::BeginMainMenuBar())
	{
		ECS::View<ManImGuiMenu> menuView(registry);
		for (const auto entityId : menuView)
		{
			ManImGuiMenu& menu = registry.getRef<ManImGuiMenu>(entityId);
			if (ImGui::BeginMenu(menu.title.c_str()))
			{
				for (auto& [name, state] : menu.items)
				{
					ImGui::MenuItem(name.data(), nullptr, &state);
				}
				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
	}
}
