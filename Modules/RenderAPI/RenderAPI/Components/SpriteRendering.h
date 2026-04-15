#pragma once

#include <Core/ManiMaths.h>
#include <Core/ECS/Entity.h>

namespace Mani
{
	struct SpriteRendering
	{
		ECS::EntityId textureId = ECS::INVALID_ID;
		Vec2f size = VEC2F::ZERO;
	};
}