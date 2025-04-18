#pragma once

#include <ECS/Entity.h>
#include <RenderAPI/Texture.h>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	struct SpriteComponent
	{
		Vec4f color = VEC4F::ONE;
		ECS::EntityId spriteHandle;
		uint16_t repeatAmount = 1;
		Vec2f pivot = { 0.5f, 0.5f };
		ETextureFiltering filteringMode = ETextureFiltering::NEAREST;
	};
}