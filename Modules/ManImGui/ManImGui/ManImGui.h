#pragma once

#include <Core/CoreFwd.h>
#include <string_view>
#include <ManImGui/Components/ManImGuiMenu.h>

namespace Mani
{
	const std::string_view LogManImGui = "LogManImGui";

	namespace ManImGuiStatics
	{
		namespace Manifold
		{
			ManImGuiMenu& getMenu(ECS::Registry& registry);
			const ManImGuiMenu& getMenu(const ECS::Registry& registry);
		}

		bool isShowing(const ECS::Registry& registry);
	}
}