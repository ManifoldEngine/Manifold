#include "ManImGuiManifoldMenuSystem.h"

#include <ManImGui/ManImGui.h>

#include <ManImGui/Components/ManImGuiManifoldMenu.h>
#include <ManImGui/Components/ManImGuiMenu.h>

using namespace Mani;

struct ManImGuiManifoldMenuSystem::Storage
{
	ECS::EntityId menuEntityId = ECS::INVALID_ID;
};

void ManImGuiManifoldMenuSystem::onInitialize(ECS::Registry& registry, World& world)
{
	auto& storage = *registry.addSingle<Storage>();
	ECS::EntityId entityId = registry.create();
	registry.add<ManImGuiManifoldMenu>(entityId);
	ManImGuiMenu& menu = *registry.add<ManImGuiMenu>(entityId);
	menu.title = "Manifold";
	storage.menuEntityId = entityId;
}

void ManImGuiManifoldMenuSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	auto& storage = *registry.getSingle<Storage>();
	registry.destroy(storage.menuEntityId);
	registry.removeSingle<Storage>();
}
