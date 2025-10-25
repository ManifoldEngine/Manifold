#include "ResourceSystem.h"

#include <Resources/Resources.h>
#include <Resources/Components/ResourceStorage.h>

using namespace Mani;

void ResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<ResourceStorage>();
}

void ResourceSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unloadAll(registry);
	registry.removeSingle<ResourceStorage>();
}
