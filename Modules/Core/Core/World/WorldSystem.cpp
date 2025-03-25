#include "WorldSystem.h"
#include <Core/World/World.h>
#include <ECS/View.h>

using namespace Mani;

ECS::EntityId Mani::WorldSystem::createWorld(ECS::Registry& registry)
{
	ECS::EntityId worldId = registry.create();
	World* world = registry.add<World>(worldId);
	world->systemContainer.initialize();
	return worldId;
}

void Mani::WorldSystem::destroyWorld(ECS::Registry& registry, ECS::EntityId entityId)
{
	if (World* world = registry.get<World>(entityId))
	{
		world->systemContainer.deinitialize();
		registry.destroy(entityId);
	}
}

void WorldSystem::tick(float deltaTime, ECS::Registry& registry)
{
	for (const ECS::EntityId entityId : ECS::View<World>(registry))
	{
		World& world = *registry.get<World>(entityId);
		world.systemContainer.tick(deltaTime);
	}
}
