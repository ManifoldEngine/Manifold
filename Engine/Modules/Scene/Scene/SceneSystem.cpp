#include "SceneSystem.h"

#include <RenderAPI/MeshComponent.h>
#include <Assets/AssetSystem.h>

#include <unordered_map>

using namespace Mani;

std::string_view SceneSystem::getName() const
{
	return "SceneSystem";
}

bool SceneSystem::shouldTick(EntityRegistry& registry) const
{
	return false;
}

void SceneSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
}

EntityId SceneSystem::spawnScene(EntityRegistry& registry, const std::filesystem::path& path, const std::filesystem::path& materialAssetPath)
{
	if (m_assetSystem.expired())
	{
		return INVALID_ID;
	}

	std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();
	std::shared_ptr<Scene> scene = assetSystem->loadJsonAsset<Scene>(path);
	if (scene == nullptr)
	{
		return INVALID_ID;
	}

	const EntityId rootNodeEntityId = spawnNode(registry, Scene::Node(), assetSystem, materialAssetPath);

	for (const auto& node : scene->nodes)
	{

		const EntityId nodeEntityId = spawnNode(registry, node, assetSystem, materialAssetPath);
		
		Transform* nodeTransform = registry.getComponent<Transform>(nodeEntityId);
		nodeTransform->parentId = rootNodeEntityId;
	}

	return rootNodeEntityId;
}

EntityId SceneSystem::spawnNode(EntityRegistry& registry, const Scene::Node& node, const std::shared_ptr<AssetSystem>& assetSystem, const std::filesystem::path& materialAssetPath)
{
	if (assetSystem == nullptr)
	{
		return INVALID_ID;
	}

	EntityId entityId = registry.create();

	Transform* transform = registry.addComponent<Transform>(entityId);
	transform->localPosition = node.localPosition;
	transform->localRotation = node.localRotation;
	transform->localScale = node.localScale;

	if (!node.meshAsset.empty())
	{
		std::shared_ptr<Mesh> mesh = assetSystem->loadJsonAsset<Mesh>(node.meshAsset);
		if (mesh == nullptr)
		{
			return INVALID_ID;
		}

		std::shared_ptr<Material> material = assetSystem->loadJsonAsset<Material>(materialAssetPath);
		if (mesh == nullptr)
		{
			return INVALID_ID;
		}

		MeshComponent* meshComponent = registry.addComponent<MeshComponent>(entityId);
		meshComponent->mesh = mesh;
		meshComponent->material = material;
	}

	return entityId;
}
