#include "ResourceSystem.h"

using namespace Mani;

void ResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<ResourceSystem::Storage>();
}

void ResourceSystem::onDeinitialize(ECS::Registry& registry)
{
	registry.removeSingle<ResourceSystem::Storage>();
}

ECS::EntityId ResourceSystem::addExtension(ECS::Registry& registry, std::shared_ptr<IResourceSystemExtension> extension)
{
	ECS::EntityId entityId = registry.create();
	ResourceSystemExtension& ext = *registry.add<ResourceSystemExtension>(entityId);
	ext.obj = extension;
	return entityId;
}

void ResourceSystem::removeExtension(ECS::Registry& registry, ECS::EntityId entityId)
{
	registry.destroy(entityId);
}