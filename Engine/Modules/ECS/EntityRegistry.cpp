#include "EntityRegistry.h"
#include "Internals/EntityContainer_Implementation.h"

using namespace ECSEngine;

ComponentId EntityRegistry::s_componentCounter = 0;


EntityRegistry::EntityRegistry()
{
	m_pEntityContainer = new EntityContainer_Implementation();
}

EntityRegistry::~EntityRegistry()
{
	delete m_pEntityContainer;
}

EntityId EntityRegistry::create()
{
	return m_pEntityContainer->create();
}

bool EntityRegistry::destroy(EntityId entityId)
{
	return m_pEntityContainer->destroy(entityId);
}

const Entity* ECSEngine::EntityRegistry::getEntity(EntityId entityId) const
{
	return m_pEntityContainer->getEntity(entityId);
}

size_t EntityRegistry::size() const
{
	return m_pEntityContainer->size();
}

bool EntityRegistry::isValid(EntityId entityId) const
{
	return m_pEntityContainer->isValid(entityId);
}

void EntityRegistry::resetComponentIds()
{
	s_componentCounter = 0;
}
