#pragma once

#include <OpenGL/OpenGL.h>
#include <RenderAPI/Texture.h>

namespace Mani
{
	class OpenGLTexture2D
	{
	public:
		bool load(const Texture& texture);
		void unload();

		void bind(uint32_t slot);
		void unbind();

		void setFilteringMode(ETextureFiltering mode);
		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
	private:
		unsigned int m_textureId = UINT32_MAX;
		int m_width = 0;
		int m_height = 0;
		int m_channels = 0;
		int m_boundSlot = -1;
		int m_filteringMode = 0;

		static int toOpenGLTextureFiltering(Mani::ETextureFiltering mode);
	};
}