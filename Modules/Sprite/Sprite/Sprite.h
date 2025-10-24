#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	constexpr std::string_view LogSprite = "Sprite";

	struct Sprite
	{
		std::string texturePath = "";
		uint32_t texelsPerUnit = 1;

		ECS::EntityId textureId = ECS::INVALID_ID;
		ECS::EntityId quadId = ECS::INVALID_ID;
	};

	struct SpriteQuadDatabase
	{
		Map<Vec2i, ECS::EntityId> quads;
	};

	namespace SpriteStatics
	{
		// attempts to configure a MeshRendering for sprite display. If resources are not ready, it will queue up the necessary loading and waiting tasks
		void loadAsyncAndAddSprite(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& spritePath, const std::string_view& shaderPath, uint32_t tag, bool withBoundingSphere = true);

		ECS::EntityId getOrAddQuad(ECS::Registry& registry, const Vec2i& size);
	}
}