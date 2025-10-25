#pragma once

#include <Core/CoreFwd.h>
#include <Core/Containers/List.h>

namespace Mani
{
	struct FModPlayQueue
	{
		List<ECS::EntityId> value;
	};
}