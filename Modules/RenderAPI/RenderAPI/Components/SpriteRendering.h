#pragma once

#include <Core/ManiMaths.h>
#include <Core/ECS/Entity.h>

namespace Mani
{
	struct SpriteRendering
	{
		EntityId textureId = INVALID_ID;
		Vec2f size = VEC2F::ZERO;
	};
}