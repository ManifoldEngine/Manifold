#include "ResourceLoader_Texture.h"

#include <Resources/Components/Resource.h>
#include <RenderAPI/Texture.h>

#include <OpenGL/STBI.h>
#include <OpenGL/OpenGLConfig.h>

using namespace Mani;

bool Mani::ResourceLoader_Texture::load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    Ref<Resource<Texture>> resource = registry.get<Resource<Texture>>(resourceId);

    uint8_t stbiSetFlipVerticallyOnLoad = Mani::STBISETFLIPVERTICALLYONLOAD_DISABLED;
    ECS::ConstView<Resource<OpenGLConfig>> openGLConfigView(registry);
    const auto it = openGLConfigView.begin();
    if (it != openGLConfigView.end())
    {
        auto config = registry.get<Resource<OpenGLConfig>>(it.getEntityId());
        stbiSetFlipVerticallyOnLoad = config->value.stbiSetFlipVerticallyOnLoad;
    }
    STBI::load(absolutePath.string(), resource->value, stbiSetFlipVerticallyOnLoad);
    return STBI::isLoaded(resource->value);
}
