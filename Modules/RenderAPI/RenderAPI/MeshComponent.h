#pragma once

#include <ECS/Entity.h>

namespace Mani
{
	struct MeshComponent
	{
		ECS::EntityId meshHandle;
		ECS::EntityId materialHandle;
	};
}