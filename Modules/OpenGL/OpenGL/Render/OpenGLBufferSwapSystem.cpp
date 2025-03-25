#include "OpenGLBufferSwapSystem.h"

#include <OpenGL/OpenGLWindowContext.h>
#include <GLFW/glfw3.h>

using namespace Mani;

void OpenGLBufferSwapSystem::tick(float deltaTime, ECS::Registry& registry)
{
	OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>();
	MANI_ASSERT(context != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");

	glfwSwapBuffers(context->window);
}
