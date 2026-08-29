#pragma once

#include <Core/CoreFwd.h>
#include <string_view>
#include <ManImGui/Components/ManImGuiMenu.h>

namespace Mani
{
	constexpr LogChannel ManImGuiLog("ManImGuiLog");

	namespace ManImGui
	{
		namespace Manifold
		{
			Ref<ManImGuiMenu> getMenu(ECS::Registry& registry);
			Ref<const ManImGuiMenu> getMenu(const ECS::Registry& registry);
		}

		bool isShowing(const ECS::Registry& registry);
	}
}