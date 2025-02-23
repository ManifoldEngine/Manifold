#pragma once

#include <RenderAPI/Texture.h>
#include <filesystem>

namespace Mani
{
	struct Sprite
	{
		std::string name;

		std::string shaderPath;
		std::string texturePath;
	};
}