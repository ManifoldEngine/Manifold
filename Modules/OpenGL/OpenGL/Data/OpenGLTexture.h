#pragma once

#include <OpenGL/OpenGL.h>
#include <OpenGL/Data/STBITexture.h>
#include <RenderAPI/Texture.h>

namespace Mani
{
	class OpenGLTexture2D
	{
	public:
		OpenGLTexture2D();
		OpenGLTexture2D(const STBITexture& texture);
		~OpenGLTexture2D();

		void bind(uint32_t slot);
		void unbind();

		void setFilteringMode(ETextureFiltering mode);
		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
	private:
		unsigned int m_textureId;
		int m_width;
		int m_height;
		int m_channels;
		int m_boundSlot;
		int m_filteringMode;

		static int toOpenGLTextureFiltering(Mani::ETextureFiltering mode);
	};
}