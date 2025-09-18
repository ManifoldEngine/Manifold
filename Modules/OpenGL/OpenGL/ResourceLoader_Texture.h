#pragma once

#include <Core/CoreFwd.h>

#include <Resources/Resource.h>

#include <RenderAPI/Texture.h>

#include <OpenGL/STBI.h>
#include <OpenGL/OpenGLConfig.h>

namespace Mani
{
    template<>
    bool ResourceLoader::load<Texture>(ECS::Registry& registry, const std::filesystem::path& absolutePath, Resource<Texture>& resource)
    {
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
}