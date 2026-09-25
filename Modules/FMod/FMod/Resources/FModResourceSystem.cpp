#include "FModResourceSystem.h"

#include <FMod/Resources/FModSound.h>
#include <FMod/Resources/FModStream.h>
#include <FMod/Resources/ResourceLoader_FModSound.h>
#include <FMod/Resources/ResourceLoader_FModStream.h>

#include <Resources/Resources.h>

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
	Resources::registerLoaderFor<FModSound, ResourceLoader_FModSound>(registry);
	Resources::registerLoaderFor<FModStream, ResourceLoader_FModStream>(registry);
	Resources::registerExtension<FModResourceExtension>(registry);
}

void FModResourceSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterExtension<FModResourceExtension>(registry);
}
