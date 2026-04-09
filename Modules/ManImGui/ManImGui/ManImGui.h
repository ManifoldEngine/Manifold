#pragma once

#include <Core/CoreFwd.h>
#include <string_view>
#include <ManImGui/Components/ManImGuiMenu.h>

namespace Mani
{
	namespace ManImGuiStatics
	{
		namespace Manifold
		{
			Ref<ManImGuiMenu> getMenu(ECS::Registry& registry);
			Ref<const ManImGuiMenu> getMenu(const ECS::Registry& registry);
		}

		bool isShowing(const ECS::Registry& registry);
	}
}