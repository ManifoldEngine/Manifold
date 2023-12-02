#pragma once

#include <OpenGL/OpenGL.h>
#include <Inputs/Interfaces/InputGenerator.h>
#include <memory>

struct GLFWwindow;

namespace Mani
{
	class OpenGLSystem;

	class OpenGLInput: public IInputGenerator
	{
	public:
		OpenGLInput(const std::weak_ptr<OpenGLSystem>& openGLSystem);
		~OpenGLInput();

		virtual std::string getName() const override;

		virtual bool consumeInputBuffer(std::vector<ButtonControl>& outBuffer) override;
		virtual bool getAxis(std::vector<AxisControl>& outAxis) override;

		virtual void onInputSystemTick(float deltaTime, EntityRegistry& registry) override;
		
		AxisControl wasd = { "WASD" };
		AxisControl mouse = { "Mouse" };

	private:
		std::weak_ptr<OpenGLSystem> m_openGLSystem;
		std::vector<ButtonControl> m_inputBuffer;

		static void glfwCallback_onMouseMoved(GLFWwindow* window, double x, double y);
	};
}

