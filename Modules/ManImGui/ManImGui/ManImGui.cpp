#include "ManImGui.h"

#include <ManImGui/Components/ManImGuiWindowContext.h>
#include <ManImGui/Components/ManImGuiMenu.h>
#include <ManImGui/Components/ManImGuiManifoldMenu.h>

using namespace Mani;

bool ManImGuiStatics::isShowing(const ECS::Registry& registry)
{
	if (const auto* context = registry.getSingle<ManImGuiWindowContext>())
	{
		return context->mode == EManImGuiMode::Show;
	}
	return false;
}

ECS::EntityId ManImGuiStatics::ManifoldMenu::getEntityId(const ECS::Registry& registry)
{
	ECS::View<ManImGuiMenu, ManImGuiManifoldMenu> view(registry);
	return view.first();
}

void ManImGuiStatics::Menu::addItem(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name)
{
    ManImGuiMenu& menu = registry.getRef<ManImGuiMenu>(entityId);
    menu.items.add(name, false);
}

void ManImGuiStatics::Menu::removeItem(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name)
{
    ManImGuiMenu& menu = registry.getRef<ManImGuiMenu>(entityId);
    menu.items.remove(name);
}

bool ManImGuiStatics::Menu::isOpened(const ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name)
{
    const ManImGuiMenu& menu = registry.getRef<ManImGuiMenu>(entityId);
    const bool* value = menu.items.find(name);
    MANI_ASSERT(value != nullptr, "no menu with name {}", name);
    return *value;
}

void ManImGuiStatics::Menu::open(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name)
{
    ManImGuiMenu& menu = registry.getRef<ManImGuiMenu>(entityId);
    bool* value = menu.items.find(name);
    MANI_ASSERT(value != nullptr, "no menu with name {}", name);
    *value = true;
}

void ManImGuiStatics::Menu::close(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& name)
{
    ManImGuiMenu& menu = registry.getRef<ManImGuiMenu>(entityId);
    bool* value = menu.items.find(name);
    MANI_ASSERT(value != nullptr, "no menu with name {}", name);
    *value = false;
}

