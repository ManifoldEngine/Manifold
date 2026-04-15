#include "FModOneShotSystem.h"

#include <Core/Async/Parallel.h>
#include <FMod/Components/FModChannel.h>
#include <FMod/Components/FModOneShot.h>

using namespace Mani;

void FModOneShotSystem::tick(ECS::Registry& registry)
{
	ECS::View<FModChannel, FModOneShot> view(registry);
	Mani::parallelFor(view, [&](ECS::EntityId entityId, FModChannel& channel, FModOneShot& oneShot)
	{
		if (channel.value == nullptr)
		{
			return;
		}

		bool isPlaying = false;
		channel.value->isPlaying(&isPlaying);
		if (!isPlaying)
		{
			registry.deferDestroy(entityId);
		}
	});
}
