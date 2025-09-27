#pragma once

#include <cstdint>
#include <string>
#include <ManiMaths/Vec2.h>

namespace Mani
{
	enum class ETextureFiltering : uint8_t
	{
		Linear, // returns a weighted average of the color
		Nearest, // returns the exact color
	};

	struct Texture
	{
		Vec2i size = { 0, 0 };
		int channels = 0;
		unsigned char* buffer = nullptr;
	};
}