#include "RenderContextSystem.h"

#include <RenderAPI/RenderContext.h>

using namespace Mani;

void RenderContextSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<RenderContext>();
}

void RenderContextSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	registry.removeSingle<RenderContext>();
}
