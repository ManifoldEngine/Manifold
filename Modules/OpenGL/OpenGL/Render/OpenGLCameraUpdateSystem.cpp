#include "OpenGLCameraUpdateSystem.h"
#include <OpenGL/OpenGLWindowContext.h>
#include <Camera/Camera.h>

using namespace Mani;

void OpenGLCameraUpdateSystem::tick(Mani::ECS::Registry& registry)
{
	const OpenGLWindowContext* context = registry.findSinglePinned<OpenGLWindowContext>();
	MANI_ASSERT(context != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");

	for (auto [entityId, camera] : ECS::View<Camera>(registry))
	{
		camera.width = static_cast<float>(context->width);
		camera.height = static_cast<float>(context->height);
	}
}
