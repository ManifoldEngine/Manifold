#include "Entity.h"

#include <Core/Containers/List.h>

namespace Mani
{
	bool ECS::isValid(ECS::EntityId entityId)
	{
		return entityId != ECS::INVALID_ID;
	}

	ECS::Entity::Entity(ECS::Index index) : m_index(index)
	{
	}

	ECS::EntityId ECS::Entity::getId() const
	{
		return ECS::calculateId(version, m_index);;
	}
	
	ECS::Index ECS::Entity::getIndex() const
	{
		return m_index;
	}

	ECS::EntityId ECS::calculateId(Version version, Index index)
	{
		return (static_cast<ECS::EntityId>(version) << VERSION_BITS) | static_cast<ECS::EntityId>(index);
	}

	ECS::Index ECS::toIndex(EntityId entityId)
	{
		return entityId & MAX_VERSION;
	}

	ECS::Version ECS::toVersion(EntityId entityId)
	{
		return entityId >> VERSION_BITS;
	}

	Mani::List<ECS::ComponentId> ECS::toComponentIds(const ECS::ComponentMask& mask)
	{
		Mani::List<ComponentId> ids;
		ids.reserve(MAX_COMPONENTS);
		for (ECS::ComponentId id = 0; id < MAX_COMPONENTS; id++)
		{
			if (mask.test(id))
			{
				ids.add(id);
			}
		}
		return ids;
	}
}
