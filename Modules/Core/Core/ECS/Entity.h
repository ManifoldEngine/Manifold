#pragma once

#include "Bitset.h"
#include <cstdint>
#include <limits>

namespace Mani 
{
	template<typename T>
	class List;

	namespace ECS
	{
		constexpr int MAX_COMPONENTS = 128;

#if MANI_WEBGL
		using EntityId = unsigned int;
		const EntityId INVALID_ID = UINT32_MAX;
#else
		using EntityId = unsigned long long;
		constexpr EntityId INVALID_ID = (std::numeric_limits<EntityId>::max)();

		using Index = unsigned int;
		constexpr Index MAX_INDEX = (std::numeric_limits<Index>::max)();
		constexpr int INDEX_BITS = sizeof(Index) * 8;

		using Version = unsigned int;
		constexpr Version MAX_VERSION = (std::numeric_limits<Version>::max)();
		constexpr int VERSION_BITS = sizeof(Version) * 8;
#endif

		using ComponentMask = Bitset<MAX_COMPONENTS>;
		using ComponentId = BitsetIndexType;
		constexpr ComponentId INVALID_COMPONENT_ID = std::numeric_limits<ComponentId>::max();

		bool isValid(EntityId entityId);

		/*
		 * An entity. It knows about its id and the components it has.
		 */
		struct Entity
		{
			Entity(ECS::Index index);

			EntityId getId() const;
			Index getIndex() const;

			bool isAlive = false;
			bool markedForDestruction = false;
			ComponentMask components;
			ComponentMask pinned;
			Version version = 0;

		private:
			Index m_index = 0;
		};

		EntityId calculateId(Version version, Index index);
		Index toIndex(EntityId entityId);
		Version toVersion(EntityId version);
		Mani::List<ComponentId> toComponentIds(const ComponentMask& mask);
	}
}
