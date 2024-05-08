#include "SceneSystem.h"

#include <RenderAPI/MeshComponent.h>
#include <Assets/AssetSystem.h>

#include <unordered_map>

using namespace Mani;

std::string_view SceneSystem::getName() const
{
	return "SceneSystem";
}

bool SceneSystem::shouldTick(ECS::Registry& registry) const
{
	return false;
}

void SceneSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
}

ECS::EntityId SceneSystem::spawnScene(ECS::Registry& registry, const std::filesystem::path& path, const std::filesystem::path& materialAssetPath)
{
	if (m_assetSystem.expired())
	{
		return ECS::INVALID_ID;
	}

	std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();
	std::shared_ptr<Scene> scene = assetSystem->loadJsonAsset<Scene>(path);
	if (scene == nullptr)
	{
		return ECS::INVALID_ID;
	}

	const ECS::EntityId rootNodeEntityId = spawnNode(registry, Scene::Node(), assetSystem, materialAssetPath);

	for (const auto& node : scene->nodes)
	{

		const ECS::EntityId nodeEntityId = spawnNode(registry, node, assetSystem, materialAssetPath);
		
		Transform* nodeTransform = registry.get<Transform>(nodeEntityId);
		nodeTransform->parentId = rootNodeEntityId;
	}

	return rootNodeEntityId;
}

ECS::EntityId SceneSystem::spawnNode(ECS::Registry& registry, const Scene::Node& node, const std::shared_ptr<AssetSystem>& assetSystem, const std::filesystem::path& materialAssetPath)
{
	if (assetSystem == nullptr)
	{
		return ECS::INVALID_ID;
	}

	ECS::EntityId entityId = registry.create();

	Transform* transform = registry.add<Transform>(entityId);
	transform->localPosition = node.localPosition;
	transform->localRotation = node.localRotation;
	transform->localScale = node.localScale;

	if (!node.meshAsset.empty())
	{
		std::shared_ptr<Mesh> mesh = assetSystem->loadJsonAsset<Mesh>(node.meshAsset);
		if (mesh == nullptr)
		{
			return ECS::INVALID_ID;
		}

		std::shared_ptr<Material> material = assetSystem->loadJsonAsset<Material>(materialAssetPath);
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
