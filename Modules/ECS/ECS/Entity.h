#pragma once

#include "ECS.h"
#include "Bitset.h"
#include <cstdint>

namespace Mani 
{
	const int MAX_COMPONENTS = 64;

#if MANI_WEBGL
	using EntityId = unsigned int;
	const EntityId INVALID_ID = UINT32_MAX;
#else
	using EntityId = size_t;
	const EntityId INVALID_ID = UINT64_MAX;
#endif

	using ComponentId = unsigned int;

	bool isValid(EntityId entityId);

	/*
	 * An entity. It knows about its id and the components it has.
	 */
	class Entity
	{
	public:
		Entity();
		Entity(const Entity& other);
		~Entity();

		EntityId id = INVALID_ID;

		// todo #2: this is kinda messed up. This means that when an entity is reused it will have the same id as in its previous life.
		bool isAlive = false;

		bool hasComponent(ComponentId componentId) const;
		bool hasComponents(const Bitset<MAX_COMPONENTS>& componentMask) const;
		void setComponentBit(ComponentId componentId);
		void resetComponentBit(ComponentId componentId);
		void resetComponentBits();

	private:
		Bitset<MAX_COMPONENTS>* m_components = nullptr;
	};
}
