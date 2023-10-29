#pragma once

#include "OpenGL.h"
#include <Core/System/System.h>
#include <Events/Event.h>

struct GLFWwindow;

namespace ECSEngine
{
	// manages the opengl implementation using glfw and glew.
	class OpenGL_API OpenGLSystem : public SystemBase
	{
	public:
		struct WindowContext
		{
			GLFWwindow* m_pWindow = nullptr;
			
			int width = 800;
			int height = 800;
			std::string_view name = "ECSEngine";
		};

		struct RendererContext
		{
			const char* vertexShaderSource = "#version 330 core\n"
				"layout(location = 0) in vec3 aPos;\n"
				"void main()\n"
				"{\n"
				"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
				"}\n";

			const char* fragmentShaderSource = "#version 330 core\n"
				"out vec4 FragColor;\n"
				"void main()\n"
				"{\n"
				"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
				"}\n";

			unsigned int shaderProgramId = -1;
		};

		DECLARE_EVENT(OnWindowClosedEvent, const WindowContext&)

		OnWindowClosedEvent onWindowClosed;

		OpenGLSystem();
		~OpenGLSystem();

		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;
	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& registry);

	private:
		WindowContext window;
		RendererContext renderer;

		std::vector<unsigned int>* vertexArrayObjectIds;

		static bool initializeRenderConfig(RendererContext& config);
		static unsigned int createVAO(const std::vector<float>& vertices, int index);

		static void glfwCallback_onWindowClosed(GLFWwindow* window);
		static void glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight);
	};
}