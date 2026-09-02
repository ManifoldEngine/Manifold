#pragma once

#include <Resources/IResourceLoader.h>

namespace Mani
{
	class ResourceLoader_FModSound : public IResourceLoader
	{
		// Inherited via IResourceLoader
		bool load(ECS::Registry& registry, const Path& absolutePath, EntityId resourceId, uint32_t tag) const override;
		bool unload(ECS::Registry& registry, EntityId resourceId) const override;
	};
}