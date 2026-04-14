#pragma once

#include <Core/CoreFwd.h>
#include <Resources/IResourceLoader.h>

namespace Mani
{
	class ResourceLoader_Sprite : public IResourceLoader
	{
		// Inherited via IResourceLoader
		bool load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const override;

		void postLoad(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, EResourceLoadMethod method, uint32_t tag) const override;
	};
}