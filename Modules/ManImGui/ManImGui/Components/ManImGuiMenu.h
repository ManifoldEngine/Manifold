#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	struct ManImGuiMenuItem;

	struct ManImGuiSubMenu
	{
		Map<std::string_view, ManImGuiMenuItem> items;

		ManImGuiMenuItem& addItem(const std::string_view& item);
		ManImGuiMenuItem& getItem(const std::string_view& item);
		const ManImGuiMenuItem& getItem(const std::string_view& item) const;
		void removeItem(const std::string_view& item);
		bool getSelected(const std::string_view& item) const;
		void setSelected(const std::string_view& item, bool value);
	};

	struct ManImGuiMenuItem
	{
		bool selected = false;
		ManImGuiSubMenu subMenu;
	};

	// root component of a menu
	struct ManImGuiMenu
	{
		std::string_view title = "";
		ManImGuiSubMenu subMenu;
	};
}