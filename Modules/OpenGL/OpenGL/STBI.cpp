#include "STBI.h"
#include <Core/Log.h>
#include <Core/ManiAssert.h>
#include <OpenGL/OpenGL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Mani;

bool STBI::load(const std::string_view& path, Texture& texture, uint8_t stbiSetFlipVerticallyOnLoad)
{
    MANI_ASSERT(!STBI::isLoaded(texture), "Overwriting a loaded texture!");
    stbi_set_flip_vertically_on_load(stbiSetFlipVerticallyOnLoad);

    texture.buffer = stbi_load(path.data(), &texture.size.x, &texture.size.y, &texture.channels, 0);
    if (stbi_failure_reason())
    {
        texture.buffer = nullptr;
        MANI_LOG_ERROR(LogOpenGL, "Could not load texture at {}, reason {}", path, stbi_failure_reason());
        return false;
    }
    return true;
}

bool Mani::STBI::isLoaded(const Texture& texture)
{
    return texture.buffer != nullptr;
}

void STBI::freeTexture(Texture& texture)
{
    stbi_image_free(texture.buffer);
    texture.buffer = nullptr;
}
