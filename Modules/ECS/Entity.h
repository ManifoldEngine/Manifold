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

	/*
	 * An entity. It knows about its id and the components it has.
	 */
	struct ECS_API Entity
	{
		EntityId id = UINT64_MAX;

		// TODO: this is kinda messed up. This means that when an entity is reused it will have the same id as in its previous life.
		bool bisAlive = false;

		// TODO: can't use std datastructure in exported dlls >:(
		std::bitset<MAX_COMPONENTS> components;

		inline bool HasComponents(const std::bitset<MAX_COMPONENTS>& componentMask) const 
		{
			return componentMask == (componentMask & components);
		};
	};
}

#pragma warning(default:4251)