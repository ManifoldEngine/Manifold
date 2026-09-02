#pragma once

#include <string_view>
#include <RenderAPI/Texture.h>

namespace Mani
{
	namespace STB
	{
		bool loadTexture(const std::string_view& path, Texture& texture, uint8_t stbiSetFlipVerticallyOnLoad = 0);
		bool isTextureLoaded(const Texture& texture);
		void freeTexture(Texture& texture);
	}
}