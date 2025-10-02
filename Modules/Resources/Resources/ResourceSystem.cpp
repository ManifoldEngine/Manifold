#include "ResourceSystem.h"

using namespace Mani;

void ResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<ResourceSystem::Storage>();
}

void ResourceSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	unloadAll(registry);
	registry.removeSingle<ResourceSystem::Storage>();
}

void Mani::ResourceSystem::registerExtension(ECS::Registry& registry, IResourceSystemExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "trying to register a null extension");
	if (Storage* storage = registry.getSingle<Storage>())
	{
		List<IResourceSystemExtension*>& extensions = storage->extensions;
		extensions.addUnique(extension);
	}
}

void Mani::ResourceSystem::unregisterExtension(ECS::Registry& registry, IResourceSystemExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "trying to unregister a null extension");
	if (Storage* storage = registry.getSingle<Storage>())
	{
		List<IResourceSystemExtension*>& extensions = storage->extensions;
		extensions.remove(extension);
	}
}

void ResourceSystem::unloadResource(ECS::Registry& registry, ECS::EntityId inEntityId)
{
	if (!registry.isValid(inEntityId))
	{
		return;
	}

	ResourceTag* resourceTag = registry.get<ResourceTag>(inEntityId);
	MANI_ASSERT(resourceTag != nullptr, "Valid resource exists without a resource tag");

	forEachExtension(registry, [&registry, inEntityId, tag = resourceTag->tag](const IResourceSystemExtension& ext)
	{
		ext.onResourceUnloaded(registry, inEntityId, tag);
	});

	registry.destroy(inEntityId);

	if (ResourceSystem::Storage* storage = registry.getSingle<ResourceSystem::Storage>())
	{
		std::lock_guard<std::mutex> lock(storage->pathToEntityMutex);
		for (const auto& [path, entityId] : storage->pathToEntityId)
		{
			if (entityId == inEntityId)
			{
				MANI_LOG(LogResources, "Unloading asset at {}", path.string());
				storage->pathToEntityId.remove(path);	
				return;
			}
		}
	}
}

void ResourceSystem::unloadAll(ECS::Registry& registry)
{
	for (const auto entityId : ECS::View<ResourceTag>(registry))
	{
		unloadResource(registry, entityId);
	}
}

void ResourceSystem::unloadTag(ECS::Registry& registry, uint32_t tag)
{
	for (const auto entityId : ECS::View<ResourceTag>(registry))
	{
		ResourceTag& resourceTag = *registry.get<ResourceTag>(entityId);
		if (resourceTag.tag == tag)
		{
			unloadResource(registry, entityId);
		}
	}
}
