#include "ManImGui.h"

#include <ManImGui/Components/ManImGuiWindowContext.h>
#include <ManImGui/Components/ManImGuiMenu.h>
#include <ManImGui/Components/ManImGuiManifoldMenu.h>

using namespace Mani;

bool ManImGuiStatics::isShowing(const ECS::Registry& registry)
{
	if (auto context = registry.findSingle<ManImGuiWindowContext>())
	{
		return context->mode == EManImGuiMode::Show;
	}
	return false;
}

Ref<ManImGuiMenu> ManImGuiStatics::Manifold::getMenu(ECS::Registry& registry)
{
	ECS::ConstView<ManImGuiMenu, ManImGuiManifoldMenu> view(registry);
	return registry.get<ManImGuiMenu>(view.begin().getEntityId());
}

Ref<const ManImGuiMenu> ManImGuiStatics::Manifold::getMenu(const ECS::Registry& registry)
{
	ECS::ConstView<ManImGuiMenu, ManImGuiManifoldMenu> view(registry);
	return registry.get<ManImGuiMenu>(view.begin().getEntityId());
}

