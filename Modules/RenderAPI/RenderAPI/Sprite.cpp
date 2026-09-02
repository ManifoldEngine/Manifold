#include "Sprite.h"

#include <Resources/Resources.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Primitives.h>
#include <RenderAPI/Components/BoundingSphere.h>
#include <RenderAPI/Components/MeshRendering.h>
#include <RenderAPI/Components/SpriteRendering.h>

using namespace Mani;

EntityId Sprites::getQuad(ECS::Registry& registry, const Vec2f& size, uint32_t tag)
{
	EntityId quadId = INVALID_ID;

	for (const auto [e, spriteQuad] : ECS::ConstPinnedView<SpriteQuad>(registry))
	{
		if (spriteQuad.size == size)
		{
			quadId = e;
			break;
		}
	}

	if (quadId == INVALID_ID)
	{
		quadId = Resources::inject<Mesh>(registry, Primitives::makeQuad(size), tag);
		registry.addPinned<SpriteQuad>(quadId, size);
	}

	return quadId;
}

void Sprites::addRendering(ECS::Registry& registry, EntityId entityId, const Vec2f& size, EntityId textureId, EntityId materialId, uint32_t tag, bool withBoundingSphere)
{
	const EntityId quadId = getQuad(registry, size, tag);

	Ref<MeshRendering> meshRendering = registry.add<MeshRendering>(entityId);
	meshRendering->meshResourceId = quadId;
	meshRendering->materialResourceId = materialId;

	Ref<SpriteRendering> spriteRendering = registry.add<SpriteRendering>(entityId);
	spriteRendering->textureId = textureId;
	spriteRendering->size = size;

	if (withBoundingSphere)
	{
		registry.add<BoundingSphere>(entityId, Math::sqrt(size.x * size.x + size.y * size.y));
	}
}
