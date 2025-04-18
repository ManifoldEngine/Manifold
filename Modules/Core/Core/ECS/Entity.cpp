#include "Entity.h"

using namespace Mani;

bool ECS::isValid(ECS::EntityId entityId)
{
	return entityId != ECS::INVALID_ID;
}

ECS::Entity::Entity(const Entity& other)
{
	id = other.id;
	isAlive = other.isAlive;
	m_components = other.m_components;
}

bool ECS::Entity::hasComponent(ComponentId componentId) const
{
	return m_components.test(componentId);
}

bool ECS::Entity::hasComponents(const Bitset<MAX_COMPONENTS>& componentMask) const
{
	return componentMask == (componentMask & m_components);
}

void ECS::Entity::setComponentBit(ComponentId componentId)
{
	m_components.set(componentId, true);
}

void ECS::Entity::resetComponentBit(ComponentId componentId)
{
	m_components.set(componentId, false);
}

void ECS::Entity::resetComponentBits()
{
	m_components.reset();
}
