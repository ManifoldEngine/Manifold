#include "SpriteSystem.h"

#include <Sprite/Sprite.h>
#include <Resources/ResourceSystem.h>
#include <Resources/Resources.h>

using namespace Mani;

void SpriteSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ResourceSystem>();
	registry.addSingle<SpriteQuadDatabase>();
	Resources::registerLoaderFor<Sprite>(registry, &spriteLoader);
}

void SpriteSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterLoaderFor<Sprite>(registry);
	registry.removeSingle<SpriteQuadDatabase>();
}
