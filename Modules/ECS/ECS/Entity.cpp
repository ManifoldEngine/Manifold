#include "Entity.h"

using namespace Mani;

bool Mani::isValid(EntityId entityId)
{
	return entityId != INVALID_ID;
}

Entity::Entity()
{
	m_components = new Bitset<MAX_COMPONENTS>();
}

Entity::Entity(const Entity& other)
	: Entity()
{
	id = other.id;
	isAlive = other.isAlive;
	*m_components = *other.m_components;
}

Entity::~Entity()
{
	delete m_components;
}

bool Entity::hasComponent(ComponentId componentId) const
{
	return m_components->test(componentId);
}

bool Entity::hasComponents(const Bitset<MAX_COMPONENTS>& componentMask) const
{
	return componentMask == (componentMask & *m_components);
}

void Entity::setComponentBit(ComponentId componentId)
{
	m_components->set(componentId, true);
}

void Entity::resetComponentBit(ComponentId componentId)
{
	m_components->set(componentId, false);
}

void Entity::resetComponentBits()
{
	m_components->reset();
}
