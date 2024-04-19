#pragma once

#include <Core/Core.h>
#include <Core/System/System.h>
// included for convenience
#include <Core/World/World.h>
#include <memory>

namespace Mani
{
	class World;

	class WorldSystem : public SystemBase
	{
	public:

		virtual std::string_view getName() const override { return "WorldSystem"; }
		virtual bool shouldTick(EntityRegistry& registry) const override { return true; }

		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(EntityRegistry& registry) override;

		std::shared_ptr<World> createWorld();
		bool destroyWorld(const std::shared_ptr<World>& world);

		void setRelevantWorld(const std::shared_ptr<World>& world);
		std::shared_ptr<World> getRelevantWorld() const;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;

	private:

		std::vector<std::shared_ptr<World>> m_worlds;
		std::shared_ptr<World> m_relevantWorld = nullptr;
	};
}