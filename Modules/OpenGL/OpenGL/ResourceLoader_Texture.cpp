#include "ResourceLoader_Texture.h"

#include <Resources/Components/Resource.h>
#include <RenderAPI/Texture.h>

#include <OpenGL/STBI.h>
#include <OpenGL/OpenGLConfig.h>

using namespace Mani;

ECS::ComponentId Mani::ResourceLoader_Texture::getComponentId(const ECS::Registry& registry) const
{
    return registry.getComponentId<Resource<Texture>>();
}

bool Mani::ResourceLoader_Texture::load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    Resource<Texture>& resource = registry.getRef<Resource<Texture>>(resourceId);

    uint8_t stbiSetFlipVerticallyOnLoad = Mani::STBISETFLIPVERTICALLYONLOAD_DISABLED;
    ECS::View<Resource<OpenGLConfig>> openGLConfigView(registry);
    const auto it = openGLConfigView.begin();
    if (it != openGLConfigView.end())
    {
        const Resource<OpenGLConfig>& configRes = *registry.get<Resource<OpenGLConfig>>(*it);
        stbiSetFlipVerticallyOnLoad = configRes.value.stbiSetFlipVerticallyOnLoad;
    }
    STBI::load(absolutePath.string(), resource.value, stbiSetFlipVerticallyOnLoad);
    return STBI::isLoaded(resource.value);
}
