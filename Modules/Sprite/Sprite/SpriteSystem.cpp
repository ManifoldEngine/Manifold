#include "SpriteSystem.h"

#include <Sprite/Sprite.h>
#include <Resources/ResourceSystem.h>
#include <Resources/Resources.h>

using namespace Mani;

void SpriteSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ResourceSystem>();
	registry.addSingle<SpriteQuadDatabase>();
	Resources::registerLoader(registry, &spriteLoader);
}

void SpriteSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterLoader(registry, &spriteLoader);
	registry.removeSingle<SpriteQuadDatabase>();
}
