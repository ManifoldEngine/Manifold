#pragma once

#include "Bitset.h"
#include <cstdint>
#include <limits>

namespace Mani 
{
	namespace ECS
	{
		const int MAX_COMPONENTS = 128;

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
#endif

		using ComponentId = BitsetIndexType;

		bool isValid(EntityId entityId);

		/*
		 * An entity. It knows about its id and the components it has.
		 */
		struct Entity
		{
			Entity() = default;
			Entity(const Entity&) = delete;
			Entity(Entity&& other) noexcept;
			
			EntityId getId() const;

			bool hasComponent(ComponentId componentId) const;
			bool hasComponents(const Bitset<MAX_COMPONENTS>& componentMask) const;
			void setComponentBit(ComponentId componentId);
			void resetComponentBit(ComponentId componentId);
			void resetComponentBits();

			void setIndex(Index index);
			Index getIndex() const;

			void setVersion(Version version);
			Version getVersion() const;

			bool isAlive = false;
		private:
			void updateId();

			EntityId m_id = INVALID_ID;
			Index m_index = 0;
			Version m_version = 0;

			Bitset<MAX_COMPONENTS> m_components;
		};

		EntityId calculateId(Index index, ECS::Version version);
		Index toIndex(EntityId entityId);
		Version toVersion(EntityId version);
	}
}
