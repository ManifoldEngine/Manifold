#pragma once

#include <Resources/IResourceLoader.h>

namespace Mani
{
	class ResourceLoader_FModStream : public IResourceLoader
	{
		// Inherited via IResourceLoader
		bool load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const override;
		void postLoad(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const override;
	};
}