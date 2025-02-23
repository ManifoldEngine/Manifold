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

void Mani::AssetSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
}

void Mani::AssetSystem::onDeinitialize(ECS::Registry& registry)
{
}

bool Mani::AssetSystem::tryGetFullPath(const std::filesystem::path& relativePath, std::filesystem::path& outPath)
{
	if (!FileSystem::tryGetRootPath(outPath))
	{
		return false;
	}

	outPath.append(relativePath.string());
	return true;
}
