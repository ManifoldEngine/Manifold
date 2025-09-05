#include "OpenGLCameraUpdateSystem.h"
#include <OpenGL/OpenGLWindowContext.h>
#include <Camera/Camera.h>

using namespace Mani;

void OpenGLCameraUpdateSystem::tick(Mani::ECS::Registry& registry)
{
	OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>();
	MANI_ASSERT(context != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");

	for (const auto entityId : ECS::View<Camera>(registry))
	{
		Camera& camera = *registry.get<Camera>(entityId);
		camera.width = static_cast<float>(context->width);
		camera.height = static_cast<float>(context->height);
	}
}
