#include "OpenGLTexture.h"
#include <Core/Log.h>
#include <GL/gl3w.h>
#include <Core/ManiAssert.h>

using namespace Mani;

OpenGLTexture2D::OpenGLTexture2D(const STBITexture& texture)
    : m_textureId(UINT32_MAX), 
    m_width(texture.width),
    m_height(texture.height),
    m_channels(texture.channels),
    m_boundSlot(-1),
    m_filteringMode(0)
{
    if (texture.data != nullptr)
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

        GLint filteringMode =
            (mode);
        glTexParameteri(m_textureId, GL_TEXTURE_MIN_FILTER, filteringMode);
        glTexParameteri(m_textureId, GL_TEXTURE_MAG_FILTER, filteringMode);

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
            texture.data
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

        m_filteringMode = GL_LINEAR;

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
            texture.data
        );
#endif
    }
    else
    {
        MANI_LOG_ERROR(LogOpenGL, "Received null texture");
    }
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

void Mani::OpenGLTexture2D::setFilteringMode(ETextureFiltering mode)
{
    GLint glMode = toOpenGLTextureFiltering(mode);
    if (glMode != m_filteringMode)
    {
        glTextureParameteri(m_textureId, GL_TEXTURE_MIN_FILTER, glMode);
        glTextureParameteri(m_textureId, GL_TEXTURE_MAG_FILTER, glMode);
        m_filteringMode = glMode;
    }
}

int Mani::OpenGLTexture2D::toOpenGLTextureFiltering(Mani::ETextureFiltering mode)
{
    switch (mode)
    {
        case ETextureFiltering::LINEAR: return GL_LINEAR;
        case ETextureFiltering::NEAREST: return GL_NEAREST;
        default: MANI_ASSERT(false, "Unknown texture filtering mode.");
    }
    return 0;
}
