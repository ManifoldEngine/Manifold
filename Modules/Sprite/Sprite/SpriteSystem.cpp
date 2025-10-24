#include "SpriteSystem.h"

#include <Sprite/Sprite.h>
#include <Resources/ResourceSystem.h>

using namespace Mani;

void SpriteSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ResourceSystem>();
	registry.addSingle<SpriteQuadDatabase>();
	ResourceSystem::registerLoader(registry, &spriteLoader);
}

void SpriteSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	ResourceSystem::unregisterLoader(registry, &spriteLoader);
	registry.removeSingle<SpriteQuadDatabase>();
}
