#pragma once

#include <atomic>
#include <filesystem>

namespace Mani
{
	// unspecialized resource component used to tag resource entities.
	struct ResourceTag 
	{
		uint32_t tag = 0;
	};

	template<typename T>
	struct Resource
	{
		T value;
	};

	struct ResourcePath
	{
		std::filesystem::path value;
	};

	struct ResourceReady {};

	template<typename T>
	struct ResourceLoader
	{
		class IResourceLoader* value = nullptr;
	};
}