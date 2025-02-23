#pragma once

#include <Events/Event.h>
#include <ECS/Registry.h>
#include <filesystem>

namespace Mani
{
	template<typename T>
	struct AssetDatabase
	{
		DECLARE_EVENT(OnAssetEvent, ECS::Registry& /*registry*/, const std::weak_ptr<T> /*asset*/);
		OnAssetEvent onAssetLoaded;

		std::unordered_map<std::filesystem::path, std::shared_ptr<T>> assets;
	};
}