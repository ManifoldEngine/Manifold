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
	registry.addSingle<AssetDatabase>();
}

void Mani::AssetSystem::onDeinitialize(ECS::Registry& registry)
{
	registry.removeSingle<AssetDatabase>();
}

bool Mani::AssetSystem::unloadJsonAsset(ECS::Registry& registry, const std::filesystem::path& relativePath)
{
	std::filesystem::path path;
	if (!tryGetFullPath(relativePath, path))
	{
		return false;
	}

	AssetDatabase* database = registry.getSingle<AssetDatabase>();
	MANI_ASSERT(database != nullptr, "Asset system was not initialized, or the asset database got deleted");
	database->jsonAssets.erase(path); 
	return true;
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
