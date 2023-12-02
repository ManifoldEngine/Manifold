#pragma once

#include <OpenGL/OpenGL.h>

namespace Mani
{
	class OpenGLTexture2D
	{
	public:
		OpenGLTexture2D(const std::string_view& path);
		~OpenGLTexture2D();

		bool isLoaded() const { return m_isLoaded; };
		void bind(uint32_t slot);
		void unbind();
	private:
		unsigned int m_textureId;
		bool m_isLoaded;
		int m_width;
		int m_height;
		int m_channels;
		int m_boundSlot;
	};
}