#include "Resources.h"

using namespace Mani;

bool Mani::Resources::isReady(const ECS::Registry& registry, ECS::EntityId entityId)
{
	return registry.has<ResourceReady>(entityId);
}

void Resources::registerExtension(ECS::Registry& registry, IResourceSystemExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "trying to register a null extension");
	Ref<ResourceStorage> storage = registry.getSingle<ResourceStorage>();
	storage->extensions.addUnique(extension);
}

void Resources::unregisterExtension(ECS::Registry& registry, IResourceSystemExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "trying to unregister a null extension");
	Ref<ResourceStorage> storage = registry.getSingle<ResourceStorage>();
	storage->extensions.remove(extension);
}

void Resources::unload(ECS::Registry& registry, ECS::EntityId inEntityId)
{
	if (!registry.isValid(inEntityId))
	{
		return;
	}

	Ref<ResourceTag> resourceTag = registry.get<ResourceTag>(inEntityId);
	
	forEachExtension(registry, [&registry, inEntityId, tag = resourceTag->tag](const IResourceSystemExtension& ext)
	{
		ext.onResourceUnloaded(registry, inEntityId, tag);
	});

	registry.deferDestroy(inEntityId);
}

void Resources::unloadAll(ECS::Registry& registry)
{
	for (const auto [entityId, _] : ECS::ConstView<ResourceTag>(registry))
	{
		unload(registry, entityId);
	}
}

void Resources::unloadTag(ECS::Registry& registry, uint32_t tag)
{
	for (auto [entityId, resourceTag] : ECS::View<ResourceTag>(registry))
	{
		if (resourceTag.tag == tag)
		{
			unload(registry, entityId);
		}
	}
}
