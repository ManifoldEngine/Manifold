#include "STBITexture.h"
#include <Core/Log.h>
#include <Core/ManiAssert.h>
#include <OpenGL/OpenGL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Mani;

bool STBITexture::load(const std::string_view& path)
{
    MANI_ASSERT(!isLoaded(), "Overwriting a loaded texture!");
    stbi_set_flip_vertically_on_load(1);

    data = stbi_load(path.data(), &width, &height, &channels, 0);
    if (stbi_failure_reason())
    {
        data = nullptr;
        MANI_LOG_ERROR(LogOpenGL, "Could not load texture at {}, reason {}", path, stbi_failure_reason());
        return false;
    }
    return true;
}

void STBITexture::freeTexture()
{
    stbi_image_free(data);
    data = nullptr;
}
