#pragma once

#include <Core/CoreFwd.h>
#include <Resources/IResourceLoader.h>

namespace Mani
{
    class ResourceLoader_Texture : public IResourceLoader
    {
        // Inherited via IResourceLoader
        bool load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const override;
        bool unload(ECS::Registry& registry, ECS::EntityId resourceId) const override;
    };
}