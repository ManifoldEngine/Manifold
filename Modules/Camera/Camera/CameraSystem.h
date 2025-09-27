#pragma once

#include <Core/CoreFwd.h>
// forwarded for convenience
#include <Camera/Camera.h>

namespace Mani
{
	class CameraSystem : public ECS::System
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(const ECS::Registry& registry) const override;
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PreUpdate; }

		virtual void tick(ECS::Registry& registry) override;
	private:
		struct Storage;
	};
}