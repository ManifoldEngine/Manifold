#include "FillableBarSystem.h"

#include <Core/Async/Parallel.h>

#include <RenderAPI/Components/MeshRendering.h>

#include <UI/UI.h>
#include <UI/Components/FillableBar.h>

using namespace Mani;

void FillableBarSystem::tick(Mani::ECS::Registry& registry)
{
	ECS::View<UI::FillableBar, MeshRendering> view(registry);
	parallelFor(view, [&](EntityId entityId, UI::FillableBar& bar, MeshRendering& meshRendering) 
	{
		meshRendering.shaderParameters[UI::ShaderNames::MANI_UI_FILLRATIO] = bar.amount;
	});
}
