#include "OpenGL.h"

using namespace Mani;

OpenGLRenderStorage& getRenderStorageChecked(ECS::Registry& registry)
{
	OpenGLRenderStorage* storage = registry.getSingle<OpenGLRenderStorage>();
	MANI_ASSERT(storage != nullptr, "OpenGLSystem is not initialized");
	return *storage;
}

void OpenGL::registerExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	MANI_ASSERT(extension != nullptr, "Cannot register a null extension");
	auto& storage = getRenderStorageChecked(registry);
	storage.extensions.addUnique(extension);
}

void OpenGL::unregisterExtension(ECS::Registry& registry, IOpenGLRenderExtension* extension)
{
	if (OpenGLRenderStorage* storage = registry.getSingle<OpenGLRenderStorage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		List<IOpenGLRenderExtension*>& extensions = storage->extensions;
		extensions.remove(extension);
	}
}

void OpenGL::registerRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	MANI_ASSERT(renderer != nullptr, "Cannot register a null renderer");

	auto& storage = getRenderStorageChecked(registry);
	List<IOpenGLRenderer*>& renderers = storage.renderers;
	renderers.addUnique(renderer);

	renderers.sort([](const IOpenGLRenderer* lhs, const IOpenGLRenderer* rhs)
	{
		return lhs->getId() < rhs->getId();
	});
}

void OpenGL::unregisterRenderer(ECS::Registry& registry, IOpenGLRenderer* renderer)
{
	if (OpenGLRenderStorage* storage = registry.getSingle<OpenGLRenderStorage>())
	{
		// no need to check on unregister as the system might have been uninitialized already.
		List<IOpenGLRenderer*>& renderers = storage->renderers;
		renderers.remove(renderer);
	}
}