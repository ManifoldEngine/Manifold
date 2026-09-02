#include "FModResourceSystem.h"

#include <FMod/Resources/FModSound.h>
#include <FMod/Resources/FModStream.h>

#include <Resources/Resources.h>
#include <Resources/ResourceSystem.h>

using namespace Mani;

void FModResourceExtension::onResourceLoaded(ECS::Registry& registry, EntityId entityId, uint32_t tag) const
{
}

void FModResourceExtension::onResourceUnloaded(ECS::Registry& registry, EntityId entityId, uint32_t tag) const
{
	if (registry.has<Resource<FModSound>>(entityId))
	{
		Ref<Resource<FModSound>> resource = registry.get<Resource<FModSound>>(entityId);
		resource->value.sound->release();
		resource->value.sound = nullptr;
	}
}

void FModResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ResourceSystem>();
	Resources::registerLoaderFor<FModSound>(registry, &soundLoader);
	Resources::registerLoaderFor<FModStream>(registry, &streamLoader);
	Resources::registerExtension(registry, &extension);
}

void FModResourceSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterLoaderFor<FModSound>(registry);
	Resources::unregisterLoaderFor<FModStream>(registry);
	Resources::unregisterExtension(registry, &extension);
}
