#pragma once

#include "OpenGL.h"
#include <Core/System/System.h>

struct GLFWwindow;

namespace Mani
{
	// manages the opengl implementation using glfw and glew.
	class OpenGLSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(ECS::Registry& registry) const override;

		virtual void tick(float deltaTime, ECS::Registry& registry) override;
		
	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

	private:
		static void terminate(ECS::Registry& registry);

		static void glfwCallback_onWindowClosed(GLFWwindow* window);
		static void glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight);
	};
}