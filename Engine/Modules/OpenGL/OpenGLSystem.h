#pragma once

#include "OpenGL.h"
#include <Core/System/System.h>
#include <Events/Event.h>

struct GLFWwindow;

namespace ECSEngine
{
	class OpenGLLayer;

	// manages the opengl implementation using glfw and glew.
	class OpenGL_API OpenGLSystem : public SystemBase
	{
	public:
		struct WindowContext
		{
			GLFWwindow* window = nullptr;
			
			int width = 800;
			int height = 800;
			std::string_view name = "ECSEngine";
		};

		DECLARE_EVENT(OnWindowClosedEvent, const WindowContext&)

		OnWindowClosedEvent onWindowClosed;

		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;
	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& registry);

	private:
		WindowContext window;

		OpenGLLayer* m_pLayer = nullptr;

		void terminate();

		static void destroyWindow(WindowContext& context);

		static void glfwCallback_onWindowClosed(GLFWwindow* window);
		static void glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight);
	};
}