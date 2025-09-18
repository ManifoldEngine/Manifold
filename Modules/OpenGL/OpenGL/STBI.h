#pragma once

#include <string_view>
#include <RenderAPI/Texture.h>

namespace Mani
{
	namespace STBI
	{
		bool load(const std::string_view& path, Texture& texture, uint8_t stbiSetFlipVerticallyOnLoad = 0);
		bool isLoaded(const Texture& texture);
		void freeTexture(Texture& texture);
	}
}