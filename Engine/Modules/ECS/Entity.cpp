#include "Entity.h"
#include <bitset>

using namespace ECSEngine;

Entity::Entity()
{
	m_pComponents = new std::bitset<MAX_COMPONENTS>();
}

Entity::Entity(const Entity& other)
	: Entity()
{
	id = other.id;
	bisAlive = other.bisAlive;
	*m_pComponents = *other.m_pComponents;
}

Entity::~Entity()
{
	delete m_pComponents;
}

bool Entity::hasComponent(ComponentId componentId) const
{
	return m_pComponents->test(componentId);
}

bool Entity::hasComponents(const std::bitset<MAX_COMPONENTS>& componentMask) const
{
	return componentMask == (componentMask & *m_pComponents);
}

void Entity::setComponentBit(ComponentId componentId)
{
	m_pComponents->set(componentId, true);
}

void Entity::resetComponentBit(ComponentId componentId)
{
	m_pComponents->set(componentId, false);
}

void Entity::resetComponentBits()
{
	m_pComponents->reset();
}
