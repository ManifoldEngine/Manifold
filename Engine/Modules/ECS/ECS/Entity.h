#pragma once

#include "ECS.h"
#include <cstdint>

namespace std 
{
	template<size_t _Bits>
	class bitset;
}

namespace Mani 
{

	const int MAX_COMPONENTS = 64;

	using EntityId = size_t;
	using ComponentId = size_t;

	const EntityId INVALID_ID = UINT64_MAX;

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

		EntityId id = UINT64_MAX;

		// todo #2: this is kinda messed up. This means that when an entity is reused it will have the same id as in its previous life.
		bool isAlive = false;

		bool hasComponent(ComponentId componentId) const;
		bool hasComponents(const std::bitset<MAX_COMPONENTS>& componentMask) const;
		void setComponentBit(ComponentId componentId);
		void resetComponentBit(ComponentId componentId);
		void resetComponentBits();

	private:
		std::bitset<MAX_COMPONENTS>* m_components = nullptr;
	};
}
