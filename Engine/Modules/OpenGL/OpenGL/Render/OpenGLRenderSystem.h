#pragma once

#include <Core/System/System.h>
#include <map>
#include <memory>

namespace Mani
{
	class EntityRegistry;
	class SystemContainer;
	class OpenGLResourceSystem;
	class CameraSystem;
	
	class OpenGLRenderSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;

	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;
		
	private:
		std::weak_ptr<OpenGLResourceSystem> m_resourceSystem;
		std::weak_ptr<CameraSystem> m_cameraSystem;
	};
}

