#include "ManImGuiECSSystem.h"

#include <ManImGui/ManImGui.h>
#include <ManImGui/ManImGuiSystem.h>
#include <ManImGui/ManImGuiManifoldMenuSystem.h>

#include "imgui.h"

constexpr std::string_view ECS_NAME = "ECS";

using namespace Mani;

bool Mani::ManImGuiECSSystem::shouldTick(const ECS::Registry& registry) const
{
	return ManImGuiStatics::isShowing(registry);
}

void Mani::ManImGuiECSSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ManImGuiSystem>();
	world.initializeDependency<ManImGuiManifoldMenuSystem>();

	{
		const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
		ManImGuiStatics::Menu::addItem(registry, debugMenuId, ECS_NAME);
	}
}

void Mani::ManImGuiECSSystem::tick(ECS::Registry& registry)
{
	const ECS::EntityId debugMenuId = ManImGuiStatics::ManifoldMenu::getEntityId(registry);
	if (!ManImGuiStatics::Menu::isOpened(registry, debugMenuId, ECS_NAME))
	{
		return;
	}

	bool isOpened = true;
	if (!ImGui::Begin("ECS", &isOpened, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (!isOpened)
	{
		ManImGuiStatics::Menu::close(registry, debugMenuId, ECS_NAME);
		ImGui::End();
		return;
	}

	ECS::View<> view(registry);
	for (const auto entityId : view)
	{
		const ECS::Entity* entity = registry.getEntity(entityId);
		const std::string title = std::format("EntityId[{}] | index[{}] | version[{}]", entityId, entity->getIndex(), entity->getVersion());
		ImGui::Text(title.c_str());
	}

	ImGui::End();
}
