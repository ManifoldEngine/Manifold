#pragma once

#include "OpenGL.h"
#include <Core/System/System.h>
#include <Events/Event.h>
#include <glm/glm.hpp>
#include <OpenGLInput.h>

struct GLFWwindow;

namespace ECSEngine
{
	// manages the opengl implementation using glfw and glew.
	class OpenGLSystem : public SystemBase, public std::enable_shared_from_this<OpenGLSystem>
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

		const WindowContext& getWindowContext() const;

		std::shared_ptr<OpenGLInput> getInputGenerator() const;

	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& registry);

	private:
		WindowContext context;

		std::shared_ptr<OpenGLInput> m_openGLInputGenerator = nullptr;

		void terminate();

		static void destroyWindow(WindowContext& context);

		static void glfwCallback_onWindowClosed(GLFWwindow* window);
		static void glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight);
	};
}