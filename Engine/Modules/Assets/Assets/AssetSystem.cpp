#include "AssetSystem.h"

using namespace ECSEngine;

std::string_view AssetSystem::getName() const
{
	return "AssetSystem";
}

bool AssetSystem::shouldTick(EntityRegistry& registry) const
{
	return false;
}
