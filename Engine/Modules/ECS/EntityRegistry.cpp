#include "EntityRegistry.h"
#include "Internals/EntityContainer_Implementation.h"

using namespace ECSEngine;

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
	return m_entityContainer->create();
}

bool EntityRegistry::destroy(EntityId entityId)
{
	return m_entityContainer->destroy(entityId);
}

const Entity* ECSEngine::EntityRegistry::getEntity(EntityId entityId) const
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
