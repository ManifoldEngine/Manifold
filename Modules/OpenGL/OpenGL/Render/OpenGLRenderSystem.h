#pragma once

#include <Core/System/System.h>
#include <RenderAPI/IRenderSystem.h>
#include <map>
#include <memory>

namespace Mani
{
	class EntityRegistry;
	class SystemContainer;
	class OpenGLResourceSystem;
	class CameraSystem;
	
	class OpenGLRenderSystem : public SystemBase, public IRenderSystem
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;

		// IRenderSystem
		virtual void getViewport(uint32_t& x, uint32_t& y, uint32_t& width, uint32_t& height) override;
		virtual void setClearColor(const glm::vec4& color) override;

	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;
		
	private:
		std::weak_ptr<OpenGLResourceSystem> m_resourceSystem;
		std::weak_ptr<CameraSystem> m_cameraSystem;

		glm::vec4 m_clearColor = glm::vec4(.1f, .1f, .1f, 1.f);
	};
}

