#include "ManImGuiRenderSystem.h"

#include "IsManImGuiDisplayed.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace Mani;

void ManImGuiRenderSystem::tick(float deltaTime, ECS::Registry& registry)
{
	for (const auto entityId : ECS::View<IsManImGuiDisplayed>(registry))
	{
		IsManImGuiDisplayed& isManImGuiDisplayed = *registry.get<IsManImGuiDisplayed>(entityId);
		if (isManImGuiDisplayed.value)
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		break;
	}
}
