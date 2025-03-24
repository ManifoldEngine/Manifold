#pragma once

#include <Core/CoreFwd.h>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	class OpenGLResourceSystem;

	class OpenGLRenderSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(ECS::Registry& registry) const override;
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PostTick; }

		virtual void tick(float deltaTime, ECS::Registry& registry) override;

	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

	private:
		std::weak_ptr<OpenGLResourceSystem> m_resourceSystem;
	};
}

