#include "FillableBarSystem.h"

#include <Core/Async/Parallel.h>

#include <RenderAPI/MeshRendering.h>

#include <UI/UI.h>
#include <UI/Components/FillableBar.h>

using namespace Mani;

void FillableBarSystem::tick(Mani::ECS::Registry& registry)
{
	Mani::ECS::View<FillableBar, Mani::MeshRendering> view(registry);
	Mani::parallelFor(view, [&](const auto entityId, size_t threadIndex) 
	{
		const FillableBar& bar = registry.getRef<FillableBar>(entityId);
		Mani::MeshRendering& meshComp = registry.getRef<Mani::MeshRendering>(entityId);
		meshComp.shaderParameters[UI::ShaderNames::MANI_UI_FILLRATIO] = bar.amount;
	});
}
