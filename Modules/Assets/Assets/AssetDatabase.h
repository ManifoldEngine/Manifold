#pragma once

#include <Assets/IJsonAsset.h>
#include <filesystem>

namespace Mani
{
	struct AssetDatabase
	{
		DECLARE_EVENT(OnJsonAssetEvent, ECS::Registry& /*registry*/, const std::weak_ptr<IJsonAsset> /*jsonAsset*/);
		OnJsonAssetEvent onJsonAssetLoaded;

		std::unordered_map<std::filesystem::path, std::shared_ptr<IJsonAsset>> jsonAssets;
	};
}