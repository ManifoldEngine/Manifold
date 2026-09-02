#pragma once

#include <Core/ECS/Entity.h>
#include <Core/ManiTypes.h>
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

	struct ResourceMetadata
	{
		Path path;
		ECS::EntityId unloaderId = ECS::INVALID_ID;
		SizeT refCount = 0;
	};

	struct ResourceReady {};

	template<typename T>
	struct ResourceLoader
	{
		class IResourceLoader* value = nullptr;
	};

	struct ResourceUnloader
	{
		class IResourceLoader* value = nullptr;
	};

	enum class EResourceLoadMethod : uint8_t
	{
		Async = 0,
		Sync,
	};
}