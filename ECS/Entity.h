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

	const int MAX_COMPONENTS = 64;

	struct ECS_API Entity
	{
		EntityId Id = UINT64_MAX;

		// TODO: this is kinda messed up. This means that when an entity is reused it will have the same id as in its previous life.
		bool IsAlive = false;

		// TODO: can't use std datastructure in exported dlls >:(
		std::bitset<MAX_COMPONENTS> Components;

		inline bool HasComponents(const std::bitset<MAX_COMPONENTS>& ComponentMask) const 
		{
			return ComponentMask == (ComponentMask & Components);
		};
	};
}

#pragma warning(default:4251)