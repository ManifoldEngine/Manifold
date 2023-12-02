#include "EntityRegistry.h"
#include "Internals/EntityContainer_Implementation.h"

using namespace Mani;

ComponentId EntityRegistry::s_componentCounter = 0;


EntityRegistry::EntityRegistry()
{
	m_entityContainer = new EntityContainer_Implementation();
}

EntityRegistry::~EntityRegistry()
{
	delete m_entityContainer;
}

EntityId EntityRegistry::create()
{
	const EntityId entityId = m_entityContainer->create();
	onEntityCreated.broadcast(*this, entityId);
	return entityId;
}

bool EntityRegistry::destroy(EntityId entityId)
{
	if (m_entityContainer->destroy(entityId))
	{
		onEntityDestroyed.broadcast(*this, entityId);
		return true;
	}
	return false;
}

const Entity* EntityRegistry::getEntity(EntityId entityId) const
{
	return m_entityContainer->getEntity(entityId);
}

size_t EntityRegistry::size() const
{
	return m_entityContainer->size();
}

bool EntityRegistry::isValid(EntityId entityId) const
{
	return m_entityContainer->isValid(entityId);
}

void EntityRegistry::resetComponentIds()
{
	s_componentCounter = 0;
}
