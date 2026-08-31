#pragma once

#include <Core/CoreFwd.h>
// forwarded for convenience
#include <Camera/Camera.h>

namespace Mani
{
	class CameraSystem : public ECS::SystemT<CameraSystem, ETick::Enabled>
	{
	public:
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender; }
		virtual void tick(ECS::Registry& registry) override;
	private:
		struct Storage;
	};
}