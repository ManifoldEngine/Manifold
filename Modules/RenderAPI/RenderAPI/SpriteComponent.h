#pragma once

#include <RenderAPI/Sprite.h>
#include <RenderAPI/Texture.h>

namespace Mani
{
	struct SpriteComponent
	{
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		std::shared_ptr<Sprite> sprite;
		uint16_t repeatAmount = 1;
		glm::vec2 pivot = glm::vec2(0.5f, 0.5f);
		ETextureFiltering filteringMode = ETextureFiltering::NEAREST;
	};
}