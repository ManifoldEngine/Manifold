#pragma once

#include <Assets/Assets.h>

#include <Core/CoreFwd.h>

#include <Assets/AssetDatabase.h>

#include <Utils/TemplateUtils.h>
#include <Core/FileSystem.h>

#include <ManiZ/ManiZ.h>
#include <Core/GLMSerialization.h>

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
		template<typename TAsset>
		static std::weak_ptr<TAsset> loadAsset(ECS::Registry& registry, const std::filesystem::path& relativePath);

		// unloads a json asset from relative path. Path is relative to the root of the project.
		template<typename TAsset>
		static bool unloadAsset(ECS::Registry& registry, const std::filesystem::path& relativePath);

	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

		static bool tryGetFullPath(const std::filesystem::path& relativePath, std::filesystem::path& outPath);
	};

	template<typename TAsset>
	inline std::weak_ptr<TAsset> AssetSystem::loadAsset(ECS::Registry& registry, const std::filesystem::path& relativePath)
	{
		std::filesystem::path path;
		if (!tryGetFullPath(relativePath, path))
		{
			return std::weak_ptr<TAsset>();
		}

		AssetDatabase<TAsset>* database = nullptr;
		if (registry.hasSingle<AssetDatabase<TAsset>>())
		{
			database = registry.getSingle<AssetDatabase<TAsset>>();
		}
		else
		{
			database = registry.addSingle<AssetDatabase<TAsset>>();
		}
		
		if (database->assets.contains(path))
		{
			return database->assets[path];
		}
		
		std::string content;
		if (!FileSystem::tryReadFile(path, content))
		{
			MANI_LOG_ERROR(LogAssets, "Could not find asset at path {}", path.string());
			return std::weak_ptr<TAsset>();
		}

		std::shared_ptr<TAsset> asset = std::make_shared<TAsset>(ManiZ::from::json<TAsset>(content));
		database->assets[path] = asset;
		database->onAssetLoaded.broadcast(registry, asset);
		return asset;
	}

	template<typename TAsset>
	inline bool Mani::AssetSystem::unloadAsset(ECS::Registry& registry, const std::filesystem::path& relativePath)
	{
		std::filesystem::path path;
		if (!tryGetFullPath(relativePath, path))
		{
			return false;
		}

		if (AssetDatabase<TAsset>* database = registry.getSingle<AssetDatabase<TAsset>>())
		{
			database->assets.erase(path);
			return true;
		}
		
		return false;
	}
}