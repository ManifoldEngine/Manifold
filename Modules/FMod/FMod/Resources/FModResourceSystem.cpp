#include "FModResourceSystem.h"

#include <FMod/Resources/FModSound.h>
#include <FMod/Resources/FModStream.h>

#include <Resources/Resources.h>
#include <Resources/ResourceSystem.h>

using namespace Mani;

void FModResourceExtension::onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
}

void FModResourceExtension::onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	const ECS::Entity* entity = registry.getEntity(entityId);
	if (entity == nullptr)
	{
		return;
	}

	const ECS::ComponentId soundComponentId = registry.getComponentId<Resource<FModSound>>();
	if (entity->hasComponent(soundComponentId))
	{
		Resource<FModSound>& resource = registry.getRef<Resource<FModSound>>(entityId);
		resource.value.sound->release();
		resource.value.sound = nullptr;
	}
}

void FModResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ResourceSystem>();
	Resources::registerLoader(registry, &soundLoader);
	Resources::registerLoader(registry, &streamLoader);
	Resources::registerExtension(registry, &extension);
}

void FModResourceSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterLoader(registry, &soundLoader);
	Resources::unregisterLoader(registry, &streamLoader);
	Resources::unregisterExtension(registry, &extension);
}
