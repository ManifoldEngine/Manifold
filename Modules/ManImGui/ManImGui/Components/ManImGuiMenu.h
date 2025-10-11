#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	struct ManImGuiMenu
	{
		std::string title = "";
		Map<std::string_view, bool> items;
	};
}