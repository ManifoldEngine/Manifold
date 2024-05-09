#pragma once

#include <Assets/Assets.h>

#include <Core/CoreFwd.h>

#include <Assets/AssetDatabase.h>
#include <Assets/Json.h>

#include <Utils/TemplateUtils.h>

#include <Core/FileSystem.h>
#include <filesystem>
#include <iostream>

namespace Mani
{
	class AssetSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const;
		virtual bool shouldTick(ECS::Registry& registry) const;

		// loads a json asset from relative path. Path is relative to the root of the project.
		template<Derived<IJsonAsset> TAsset>
		static std::weak_ptr<TAsset> loadJsonAsset(ECS::Registry& registry, const std::filesystem::path& relativePath);

		// loads a json asset from relative path. Path is relative to the root of the project.
		template<Derived<IJsonAsset> TAsset>
		static std::weak_ptr<TAsset> loadJsonAssetrfl(ECS::Registry& registry, const std::filesystem::path& relativePath);


		// unloads a json asset from relative path. Path is relative to the root of the project.
		static bool unloadJsonAsset(ECS::Registry& registry, const std::filesystem::path& relativePath);

	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

		static bool tryGetFullPath(const std::filesystem::path& relativePath, std::filesystem::path& outPath);
	};

	template<Derived<IJsonAsset> TAsset>
	inline std::weak_ptr<TAsset> AssetSystem::loadJsonAsset(ECS::Registry& registry, const std::filesystem::path& relativePath)
	{
		std::filesystem::path path;
		if (!tryGetFullPath(relativePath, path))
		{
			return std::weak_ptr<TAsset>();
		}

		AssetDatabase* database = registry.getSingle<AssetDatabase>();
		MANI_ASSERT(database != nullptr, "Asset system was not initialized, or the asset database got deleted");
		if (database->jsonAssets.contains(path))
		{
			return dynamic_pointer_cast<TAsset>(database->jsonAssets[path]);
		}
		
		std::string content;
		if (!FileSystem::tryReadFile(path, content))
		{
			MANI_LOG_ERROR(LogAssets, "Could not find asset at path {}", path.string());
			return std::weak_ptr<TAsset>();
		}

		std::shared_ptr<TAsset> asset = std::make_shared<TAsset>();
		asset->parse(content);

		database->jsonAssets[path] = asset;
		database->onJsonAssetLoaded.broadcast(registry, asset);
		return asset;
	}

	template<Derived<IJsonAsset> TAsset>
	inline std::weak_ptr<TAsset> AssetSystem::loadJsonAssetrfl(ECS::Registry& registry, const std::filesystem::path& relativePath)
	{
		std::filesystem::path path;
		if (!tryGetFullPath(relativePath, path))
		{
			return std::weak_ptr<TAsset>();
		}

		AssetDatabase* database = registry.getSingle<AssetDatabase>();
		MANI_ASSERT(database != nullptr, "Asset system was not initialized, or the asset database got deleted");
		if (database->jsonAssetsrfl.contains(path))
		{
			return std::dynamic_pointer_cast<TAsset>(database->jsonAssetsrfl[path]);
		}

		std::string content;
		if (!FileSystem::tryReadFile(path, content))
		{
			MANI_LOG_ERROR(LogAssets, "Could not find asset at path {}", path.string());
			return std::weak_ptr<TAsset>();
		}

		std::shared_ptr<TAsset> asset = std::make_shared<TAsset>(Json<TAsset>::fromJson(content));
		database->jsonAssetsrfl[path] = asset;
		return asset;
	}
}