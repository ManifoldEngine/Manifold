#pragma once

#include "OpenGL.h"
#include <Core/ECS/System.h>

struct GLFWwindow;

namespace Mani
{
	// manages the opengl implementation using glfw and glew.
	class OpenGLSystem : public ECS::System
	{
	public:
		virtual std::string_view getName() const override { return "OpenGLSystem"; }
		virtual bool shouldTick(const ECS::Registry& registry) const { return false; }

	protected:
		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry, World& world) override;
	};
}