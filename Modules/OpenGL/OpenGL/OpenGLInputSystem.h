#pragma once

#include <OpenGL/OpenGL.h>
#include <Core/CoreFwd.h>
#include <Inputs/Cursor.h>

struct GLFWwindow;

namespace Mani
{
	class OpenGLSystem;
	
	class OpenGLInputSystem: public SystemBase
	{
	public:
		virtual std::string_view getName() const override { return "OpenGLInputSystem"; }
		virtual bool shouldTick(ECS::Registry& registry) const override { return true; }
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PreTick; }

		virtual void tick(float deltaTime, ECS::Registry& registry) override;
		
		static int maniToGLFWCursorMode(Cursor::EMode mode);
	protected:
		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

	private:
		static void glfwCallback_onMouseMoved(GLFWwindow* window, double x, double y);
		static Vec2f s_mouse;
	};
}

