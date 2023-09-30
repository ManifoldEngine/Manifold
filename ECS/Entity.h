#pragma once

#include "ECS.h"
#include <cstdint>
#include <bitset>

// TODO: do not use STL for exposed classes.
// warning C4251: 'ECSEngine::EntityRegistry::ComponentStore': class 'std::vector<unsigned char,std::allocator<unsigned char>>' needs to have dll-interface to be used by clients of class 'ECSEngine::EntityRegistry'
#pragma warning(disable:4251)

namespace ECSEngine {
	using EntityId = size_t;
	using ComponentId = size_t;

#define INVALID_ID UINT64_MAX;
	const int MAX_COMPONENTS = 64;

	struct ECS_API Entity
	{
		EntityId Id = INVALID_ID;

		// TODO: can't use std datastructure in exported dlls >:(
		std::bitset<MAX_COMPONENTS> Components;
	};
}

#pragma warning(default:4251)