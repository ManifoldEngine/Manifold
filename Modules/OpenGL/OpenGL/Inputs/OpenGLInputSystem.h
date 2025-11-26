#pragma once

#include <OpenGL/OpenGL.h>
#include <Core/CoreFwd.h>
#include <Inputs/Cursor.h>

#include <Inputs/Inputs.h>
#include <Inputs/Components/InputDevice.h>

struct GLFWwindow;

namespace Mani
{
	class OpenGLSystem;
	
	class OpenGLInputSystem: public ECS::System
	{
	public:
		virtual std::string_view getName() const override { return "OpenGLInputSystem"; }
		virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }
		virtual ETickGroup getTickGroup() const override { return ETickGroup::PreInput; }

	protected:
		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry, World& world) override;

	public:
		virtual void tick(ECS::Registry& registry) override;

	private:
		struct Storage;
	};
}

