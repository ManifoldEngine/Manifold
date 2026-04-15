#include "OpenGL.h"

using namespace Mani;

void OpenGL::registerExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "Cannot register a null extension");
	auto& storage = registry.getSinglePinned<OpenGLRenderStorage>();
	storage.extensions.addUnique(extension);
}

void OpenGL::unregisterExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	if (OpenGLRenderStorage* storage = registry.findSinglePinned<OpenGLRenderStorage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		List<IOpenGLRenderExtension*>& extensions = storage->extensions;
		extensions.remove(extension);
	}
}

void OpenGL::registerRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	MANI_ASSERT(renderer != nullptr, "Cannot register a null renderer");

	auto& storage = registry.getSinglePinned<OpenGLRenderStorage>();
	List<IOpenGLRenderer*>& renderers = storage.renderers;
	renderers.addUnique(renderer);

	renderers.sort([](const IOpenGLRenderer* lhs, const IOpenGLRenderer* rhs)
	{
		return lhs->getId() < rhs->getId();
	});
}

void OpenGL::unregisterRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	if (OpenGLRenderStorage* storage = registry.findSinglePinned<OpenGLRenderStorage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		List<IOpenGLRenderer*>& renderers = storage->renderers;
		renderers.remove(renderer);
	}
}