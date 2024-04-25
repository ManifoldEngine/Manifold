#pragma once

#include <OpenGL/OpenGL.h>
#include <Core/CoreFwd.h>
#include <memory>

struct GLFWwindow;

namespace Mani
{
	class OpenGLSystem;

	class OpenGLInputSystem: public SystemBase
	{
	public:
		virtual std::string_view getName() const override { return "OpenGLInputSystem"; }
		virtual bool shouldTick(EntityRegistry& registry) const override { return true; }
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PreTick; }

		virtual void tick(float deltaTime, EntityRegistry& registry) override;
		
	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(EntityRegistry& registry) override;

	private:
		std::weak_ptr<OpenGLSystem> m_openGLSystem;
		
		static glm::vec2 s_mouse;
		static void glfwCallback_onMouseMoved(GLFWwindow* window, double x, double y);
	};
}

