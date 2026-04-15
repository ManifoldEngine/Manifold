#pragma once

#include <Core/CoreFwd.h>
#include <Resources/Resources.h>

namespace Mani
{
	constexpr LogChannel LogSprite("Sprite");

	struct SpriteQuad 
	{
		Vec2f size = VEC2F::ZERO;
	};

	namespace Sprites
	{
		ECS::EntityId getQuad(ECS::Registry& registry, const Vec2f& size, uint32_t tag = Mani::GLOBAL_RESOURCE_TAG);
		void addRendering(ECS::Registry& registry, ECS::EntityId entityId, const Vec2f& size, ECS::EntityId textureId, ECS::EntityId materialId, uint32_t tag = Mani::GLOBAL_RESOURCE_TAG, bool withBoundingSphere = true);
	}
}