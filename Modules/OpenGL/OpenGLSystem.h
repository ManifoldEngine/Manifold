#pragma once

#include "OpenGL.h"
#include <Core/System/System.h>
#include <memory>

struct GLFWwindow;

namespace ECSEngine
{
	class OpenGL_API OpenGLSystem : public SystemBase
	{
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;
	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& registry);

	private:
		GLFWwindow* m_pWindow = nullptr;
	};
}