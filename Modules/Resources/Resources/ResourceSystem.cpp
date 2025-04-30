#include "ResourceSystem.h"

using namespace Mani;

void ResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<ResourceSystem::Storage>();
}

void ResourceSystem::onDeinitialize(ECS::Registry& registry)
{
	unloadAll(registry);
	for (const auto entityId : ECS::View<ResourceSystemExtension>(registry))
	{
		removeExtension(registry, entityId);
	}
	registry.removeSingle<ResourceSystem::Storage>();
}

ECS::EntityId ResourceSystem::addExtension(ECS::Registry& registry, std::unique_ptr<IResourceSystemExtension> extension)
{
	ECS::EntityId entityId = registry.create();
	ResourceSystemExtension& ext = *registry.add<ResourceSystemExtension>(entityId);
	ext.obj = std::move(extension);
	return entityId;
}

void ResourceSystem::removeExtension(ECS::Registry& registry, ECS::EntityId entityId)
{
	if (ResourceSystemExtension* ext = registry.get<ResourceSystemExtension>(entityId))
	{
		ext->obj.reset();
	}
	registry.destroy(entityId);
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

	ResourceSystem::Storage& storage = *registry.getSingle<ResourceSystem::Storage>();
	registry.destroy(inEntityId);

	{
		std::lock_guard<std::mutex> lock(storage.pathToEntityMutex);
		for (const auto& [path, entityId] : storage.pathToEntityId)
		{
			if (entityId == inEntityId)
			{
				MANI_LOG(LogResources, "Unloading asset at {}", path.string());
				storage.pathToEntityId.erase(path);	
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
