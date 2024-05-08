#pragma once

#include <Core/CoreFwd.h>
#include <Scene/Scene.h>
#include <filesystem>

namespace Mani
{
	class AssetSystem;

	class SceneSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(ECS::Registry& registry) const override;

		// todo #14: remove the material asset parameter.
		ECS::EntityId spawnScene(ECS::Registry& registry, const std::filesystem::path& path, const std::filesystem::path& materialAssetPath);

	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
	
	private:
		std::weak_ptr<AssetSystem> m_assetSystem;
		
		// todo #14: remove the material asset parameter.
		ECS::EntityId spawnNode(ECS::Registry& registry, const Scene::Node& node, const std::shared_ptr<AssetSystem>& assetSystem, const std::filesystem::path& materialAssetPath);
	};
}