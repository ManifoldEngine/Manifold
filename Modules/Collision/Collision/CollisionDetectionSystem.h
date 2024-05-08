#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class CollisionDetectionSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(ECS::Registry& registry) const override;

		virtual tick(float deltaTime, ECS::Registry& registry) override;

	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;
	};
}