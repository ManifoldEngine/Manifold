#include "SpriteSystem.h"

#include <Sprite/Sprite.h>

using namespace Mani;

void SpriteSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<SpriteQuadDatabase>();
}

void SpriteSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	registry.removeSingle<SpriteQuadDatabase>();
}
