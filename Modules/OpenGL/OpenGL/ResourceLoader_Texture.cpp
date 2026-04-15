#include "ResourceLoader_Texture.h"

#include <Resources/Components/Resource.h>
#include <RenderAPI/Texture.h>

#include <OpenGL/STBI.h>
#include <OpenGL/OpenGLConfig.h>

using namespace Mani;

bool Mani::ResourceLoader_Texture::load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    Resource<Texture>& resource = registry.getPinned<Resource<Texture>>(resourceId);

    uint8_t stbiSetFlipVerticallyOnLoad = Mani::STBISETFLIPVERTICALLYONLOAD_DISABLED;
    ECS::ConstPinnedView<Resource<OpenGLConfig>> openGLConfigView(registry);
    const auto it = openGLConfigView.begin();
    if (it != openGLConfigView.end())
    {
        auto& config = registry.getPinned<Resource<OpenGLConfig>>(it.getEntityId());
        stbiSetFlipVerticallyOnLoad = config.value.stbiSetFlipVerticallyOnLoad;
    }
    STBI::load(absolutePath.string(), resource.value, stbiSetFlipVerticallyOnLoad);
    return STBI::isLoaded(resource.value);
}
