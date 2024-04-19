#pragma once

#include <Core/CoreFwd.h>
#include <set>
#include <map>

namespace Mani
{
	class TransformSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;
		virtual ETickGroup getTickGroup() const override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;

		static void updateTransform(EntityRegistry& registry, EntityId entityId);

	private:
		// keep set and map allocated since the same amount of transform will be processed each tick.
		std::set<EntityId> m_processedTransforms;
		std::map<EntityId, std::set<EntityId>> m_childrenMap;

		void updateTransformsAndTheirChildrenRecursively(EntityRegistry& registry, EntityId entityId);
	};
}