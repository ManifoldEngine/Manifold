#include "ResourceLoader_Texture.h"

#include <Resources/Components/Resource.h>
#include <RenderAPI/Texture.h>

#include <OpenGL/OpenGLConfig.h>

#include <STB/STBConfig.h>
#include <STB/Textures/STBImage.h>

using namespace Mani;

bool ResourceLoader_Texture::load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    MANI_LOG(Log, "loading texture {}", resourceId);
    Resource<Texture>& resource = registry.getPinned<Resource<Texture>>(resourceId);

    uint8_t stbiSetFlipVerticallyOnLoad = Mani::STBISETFLIPVERTICALLYONLOAD_DISABLED;
    ECS::ConstPinnedView<Resource<Mani::STB::Config>> openGLConfigView(registry);
    const auto it = openGLConfigView.begin();
    if (it.isValid())
    {
        auto& config = registry.getPinned<Resource<Mani::STB::Config>>(it.getEntityId());
        stbiSetFlipVerticallyOnLoad = config.value.stbiSetFlipVerticallyOnLoad;
    }
    STB::loadTexture(absolutePath.string(), resource.value, stbiSetFlipVerticallyOnLoad);
    return STB::isTextureLoaded(resource.value);
}

bool ResourceLoader_Texture::unload(ECS::Registry& registry, ECS::EntityId resourceId) const
{
    MANI_LOG(Log, "unloading texture {}", resourceId);
    Resource<Texture>& resource = registry.getPinned<Resource<Texture>>(resourceId);
    if (STB::isTextureLoaded(resource.value))
    {
        STB::freeTexture(resource.value);
    }
    return !STB::isTextureLoaded(resource.value);
}
