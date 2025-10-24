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

ManImGuiMenu& ManImGuiStatics::Manifold::getMenu(ECS::Registry& registry)
{
	ECS::View<ManImGuiMenu, ManImGuiManifoldMenu> view(registry);
	return registry.getRef<ManImGuiMenu>(view.first());
}

const ManImGuiMenu& ManImGuiStatics::Manifold::getMenu(const ECS::Registry& registry)
{
	ECS::View<ManImGuiMenu, ManImGuiManifoldMenu> view(registry);
	return registry.getRef<ManImGuiMenu>(view.first());
}

