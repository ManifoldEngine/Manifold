#include "ManImGuiMenu.h"

using namespace Mani;

ManImGuiMenuItem& ManImGuiSubMenu::addItem(const std::string_view& item)
{
	return items.getOrAdd(item, {});
}

ManImGuiMenuItem& ManImGuiSubMenu::getItem(const std::string_view& item)
{
	return items.get(item);
}

const ManImGuiMenuItem& ManImGuiSubMenu::getItem(const std::string_view& item) const
{
	return items.get(item);
}

void ManImGuiSubMenu::removeItem(const std::string_view& item)
{
	items.remove(item);
}

bool ManImGuiSubMenu::getSelected(const std::string_view& item) const
{
	return items.get(item).selected;
}

void Mani::ManImGuiSubMenu::setSelected(const std::string_view& item, bool value)
{
	items.get(item).selected = value;
}
