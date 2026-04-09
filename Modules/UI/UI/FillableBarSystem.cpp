#include "FillableBarSystem.h"

#include <Core/Async/Parallel.h>

#include <RenderAPI/MeshRendering.h>

#include <UI/UI.h>
#include <UI/Components/FillableBar.h>

using namespace Mani;

void FillableBarSystem::tick(Mani::ECS::Registry& registry)
{
	ECS::View<FillableBar, MeshRendering> view(registry);
	parallelFor(view, [&](ECS::EntityId entityId, FillableBar& bar, MeshRendering& meshRendering) 
	{
		meshRendering.shaderParameters[UI::ShaderNames::MANI_UI_FILLRATIO] = bar.amount;
	});
}
