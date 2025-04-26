#include "STBITexture.h"
#include <Core/Log.h>
#include <OpenGL/OpenGL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Mani;

STBITexture::STBITexture()
    : data(nullptr), width(0), height(0), channels(0)
{
}

STBITexture::STBITexture(const std::string_view& path)
    : data(nullptr), width(0), height(0), channels(0)
{
    stbi_set_flip_vertically_on_load(1);

    data = stbi_load(path.data(), &width, &height, &channels, 0);
    if (stbi_failure_reason())
    {
        MANI_LOG_ERROR(LogOpenGL, "Could not load texture at {}, reason {}", path, stbi_failure_reason());
    }
}

STBITexture::~STBITexture()
{
	stbi_image_free(data);
}