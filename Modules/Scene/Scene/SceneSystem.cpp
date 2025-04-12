#include "SceneSystem.h"

#include <RenderAPI/MeshComponent.h>
#include <Assets/AssetSystem.h>

#include <unordered_map>

#include <ManiZ/ManiZ.h>

using namespace Mani;

std::string_view SceneSystem::getName() const
{
	return "SceneSystem";
}

bool SceneSystem::shouldTick(ECS::Registry& registry) const
{
	return false;
}

void SceneSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<AssetSystem>();
}

ECS::EntityId SceneSystem::spawnScene(ECS::Registry& registry, const std::filesystem::path& path, const std::filesystem::path& materialAssetPath)
{
	std::shared_ptr<Scene> scene = AssetSystem::loadAsset<Scene>(registry, path).lock();
	if (scene == nullptr)
	{
		return ECS::INVALID_ID;
	}

	const ECS::EntityId rootNodeEntityId = spawnNode(registry, Scene::Node(), materialAssetPath);

	for (const auto& node : scene->nodes)
	{

		const ECS::EntityId nodeEntityId = spawnNode(registry, node, materialAssetPath);
		
		Transform* nodeTransform = registry.get<Transform>(nodeEntityId);
		// nodeTransform->parentId = rootNodeEntityId; deprecated
	}

	return rootNodeEntityId;
}

ECS::EntityId SceneSystem::spawnNode(ECS::Registry& registry, const Scene::Node& node, const std::filesystem::path& materialAssetPath)
{
	ECS::EntityId entityId = registry.create();

	Transform* transform = registry.add<Transform>(entityId);
	/*transform->localPosition = node.localPosition;
	transform->localRotation = node.localRotation;
	transform->localScale = node.localScale;*/
	// deprecated
	if (!node.meshAsset.empty())
	{
		std::shared_ptr<Mesh> mesh = AssetSystem::loadAsset<Mesh>(registry, node.meshAsset).lock();
		if (mesh == nullptr)
		{
			return ECS::INVALID_ID;
		}

		std::shared_ptr<Material> material = AssetSystem::loadAsset<Material>(registry, materialAssetPath).lock();
		if (mesh == nullptr)
		{
			return ECS::INVALID_ID;
		}

		MeshComponent* meshComponent = registry.add<MeshComponent>(entityId);
		meshComponent->mesh = mesh;
		meshComponent->material = material;
	}

	return entityId;
}
