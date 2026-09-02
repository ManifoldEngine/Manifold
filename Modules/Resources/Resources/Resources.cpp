#include "Resources.h"

using namespace Mani;

bool Mani::Resources::isReady(const ECS::Registry& registry, EntityId entityId)
{
	return registry.hasPinned<ResourceReady>(entityId);
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

void Resources::unload(ECS::Registry& registry, EntityId inEntityId)
{
	constexpr bool checkForDeferredDestruction = true;
	if (!registry.isValid(inEntityId, checkForDeferredDestruction))
	{
		return;
	}

	ResourceMetadata& metadata = registry.getPinned<ResourceMetadata>(inEntityId);
	MANI_LOG(LogResources, "Unloading resource at {}", metadata.path.string());
	if (metadata.unloaderId != INVALID_ID)
	{
		const ResourceUnloader& unloader = registry.getPinned<ResourceUnloader>(metadata.unloaderId);
		MANI_ASSERT(unloader.value != nullptr, "registered a null unloader for asset at {}", metadata.path.string());
		unloader.value->unload(registry, inEntityId);
	}

	const ResourceTag& resourceTag = registry.getPinned<ResourceTag>(inEntityId);
	forEachExtension(registry, [&registry, inEntityId, tag = resourceTag.tag](const IResourceSystemExtension& ext)
	{
		ext.onResourceUnloaded(registry, inEntityId, tag);
	});

	registry.deferDestroy(inEntityId);
}

void Resources::unloadAll(ECS::Registry& registry)
{
	for (const auto [entityId, _] : ECS::ConstPinnedView<ResourceTag>(registry))
	{
		unload(registry, entityId);
	}
}

void Resources::unloadTag(ECS::Registry& registry, uint32_t tag)
{
	for (auto [entityId, resourceTag] : ECS::PinnedView<ResourceTag>(registry))
	{
		if (resourceTag.tag == tag)
		{
			unload(registry, entityId);
		}
	}
}
