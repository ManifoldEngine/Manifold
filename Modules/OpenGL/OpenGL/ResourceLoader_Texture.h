#pragma once

#include <Core/CoreFwd.h>
#include <Resources/IResourceLoader.h>

namespace Mani
{
    class ResourceLoader_Texture : public IResourceLoader
    {
        // Inherited via IResourceLoader
        ECS::ComponentId getComponentId(const ECS::Registry& registry) const override;
        bool load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const override;
    };
}