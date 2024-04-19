#include "OpenGLTexture.h"
#include <Core/Log.h>
#include <GL/glew.h>
#include <Core/ManiAssert.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Mani;

OpenGLTexture2D::OpenGLTexture2D(const std::string_view& path)
    : m_textureId(UINT32_MAX), 
    m_isLoaded(false),
    m_width(0),
    m_height(0),
    m_channels(0),
    m_boundSlot(-1)
{
    //stbi_set_flip_vertically_on_load(1);
   
    stbi_uc* imageData = stbi_load(path.data(), &m_width, &m_height, &m_channels, 0);
    if (imageData != nullptr)
    {
        GLenum internalFormat;
        GLenum imageDataFormat;
        switch (m_channels)
        {
            case 3:
            {
                // internal format should use the 8 bytes format
                internalFormat = GL_RGB8;
                // stb_image uses legacy format
                imageDataFormat = GL_RGB;
                break;
            }
            case 4:
            {
                // internal format should use the 8 bytes format
                internalFormat = GL_RGBA8;
                // stb_image uses legacy format
                imageDataFormat = GL_RGBA;
                break;
            }
            default:
            {
                MANI_ASSERT(false, "Unspported texture format");
                break;
            }
        }

#ifdef MANI_WEBGL
        glGenTextures(1, &m_textureId);
        glTexStorage2D(m_textureId, 1, internalFormat, m_width, m_height);

        // set texture parameters
        glTexParameteri(m_textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(m_textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(m_textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // generate texture from image data.
        glTexSubImage2D(
            m_textureId,
            0, // level
            0, // xOffset
            0, // yOffset
            m_width,
            m_height,
            imageDataFormat,
            GL_UNSIGNED_BYTE, // data type (stbi_uc is in bytes)
            imageData
        );
#else
        // create texture data
        glCreateTextures(GL_TEXTURE_2D, 1, &m_textureId);
        glTextureStorage2D(m_textureId, 1, internalFormat, m_width, m_height);
        
        // set texture parameters
        glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureParameteri(m_textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // generate texture from image data.
        glTextureSubImage2D(
            m_textureId,
            0, // level
            0, // xOffset
            0, // yOffset
            m_width,
            m_height,
            imageDataFormat,
            GL_UNSIGNED_BYTE, // data type (stbi_uc is in bytes)
            imageData
        );
#endif
        
        m_isLoaded = true;
    }
    else
    {
        MANI_LOG_ERROR(LogOpenGL, "Could not load texture at {}", path);
    }
    stbi_image_free(imageData);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_textureId);
}

void OpenGLTexture2D::bind(uint32_t slot)
{
    m_boundSlot = slot;
#if MANI_WEBGL
    glActiveTexture(GL_TEXTURE0 + m_boundSlot);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
#else
    glBindTextureUnit(m_boundSlot, m_textureId);
#endif
}

void Mani::OpenGLTexture2D::unbind()
{
    if (m_boundSlot >= 0)
    {
#if MANI_WEBGL
        glActiveTexture(GL_TEXTURE0 + m_boundSlot);
        glBindTexture(GL_TEXTURE_2D, NULL);
#else
        glBindTextureUnit(m_boundSlot, NULL);
#endif
        m_boundSlot = -1;
    }
}
