#include "AssetSystem.h"

using namespace Mani;

std::string_view AssetSystem::getName() const
{
	return "AssetSystem";
}

bool AssetSystem::shouldTick(ECS::Registry& registry) const
{
	return false;
}
