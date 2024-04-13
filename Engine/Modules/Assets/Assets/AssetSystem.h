#pragma once

#include <Assets/Assets.h>

#include <Core/Log.h>
#include <Core/System/System.h>
#include <Core/FileSystem.h>

#include <Events/Event.h>

#include <Core/Interfaces/IJsonAsset.h>

#include <Utils/TemplateUtils.h>

#include <filesystem>
#include <memory>
#include <iostream>

namespace Mani
{
	class AssetSystem : public SystemBase
	{
	public:
		DECLARE_EVENT(OnJsonAssetEvent, const std::shared_ptr<IJsonAsset> /*jsonAsset*/);
		OnJsonAssetEvent onJsonAssetLoaded;

		virtual std::string_view getName() const;
		virtual bool shouldTick(EntityRegistry& registry) const;

		// loads a json asset from relative path. Path is relative to the root of the project.
		template<Derived<IJsonAsset> TAsset>
		std::shared_ptr<TAsset> loadJsonAsset(const std::filesystem::path& relativePath);
	};

	template<Derived<IJsonAsset> TAsset>
	inline std::shared_ptr<TAsset> AssetSystem::loadJsonAsset(const std::filesystem::path& relativePath)
	{
		std::filesystem::path path;
		if (!FileSystem::tryGetRootPath(path))
		{
			return nullptr;
		}

		path.append(relativePath.string());

		std::string content;
		if (!FileSystem::tryReadFile(path, content))
		{
			MANI_LOG_ERROR(LogAssets, "Could not find asset at path {}", path.string());
			return nullptr;
		}

		std::shared_ptr<TAsset> asset = std::make_shared<TAsset>();
		asset->parse(content);

		onJsonAssetLoaded.broadcast(asset);
		return asset;
	}
}