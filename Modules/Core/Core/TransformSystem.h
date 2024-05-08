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
		virtual bool shouldTick(ECS::Registry& registry) const override;
		virtual ETickGroup getTickGroup() const override;

		virtual void tick(float deltaTime, ECS::Registry& registry) override;

		static void updateTransform(ECS::Registry& registry, ECS::EntityId entityId);

	private:
		// keep set and map allocated since the same amount of transform will be processed each tick.
		std::set<ECS::EntityId> m_processedTransforms;
		std::map<ECS::EntityId, std::set<ECS::EntityId>> m_childrenMap;

		void updateTransformsAndTheirChildrenRecursively(ECS::Registry& registry, ECS::EntityId entityId);
	};
}