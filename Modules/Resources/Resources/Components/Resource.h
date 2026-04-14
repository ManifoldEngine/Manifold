#pragma once

#include <Core/ManiTypes.h>
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
		Path value;
	};

	struct ResourceReady {};

	template<typename T>
	struct ResourceLoader
	{
		class IResourceLoader* value = nullptr;
	};

	enum class EResourceLoadMethod : uint8_t
	{
		Async = 0,
		Sync,
	};
}