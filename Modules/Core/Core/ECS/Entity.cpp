#include "Entity.h"

using namespace Mani;

bool ECS::isValid(ECS::EntityId entityId)
{
	return entityId != ECS::INVALID_ID;
}

ECS::Entity::Entity(Entity&& other) noexcept
{
	isAlive = other.isAlive;
	m_id = other.m_id;
	m_index = other.m_index;
	m_version = other.m_version;
	m_components = other.m_components;

	other.isAlive = false;
	other.m_id = INVALID_ID;
	other.m_index = 0;
	other.m_version = 0;
	other.m_components.reset();
}

ECS::EntityId ECS::Entity::getId() const
{
	return m_id;
}

bool ECS::Entity::hasComponent(ComponentId componentId) const
{
	return m_components.test(componentId);
}

bool ECS::Entity::hasComponents(const ComponentMask& componentMask) const
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

void ECS::Entity::setIndex(ECS::Index index)
{
	m_index = index;
	updateId();
}

ECS::Index ECS::Entity::getIndex() const
{
	return m_index;
}

void ECS::Entity::setVersion(ECS::Version version)
{
	m_version = version;
	updateId();
}

ECS::Version ECS::Entity::getVersion() const
{
	return m_version;
}

void ECS::Entity::updateId()
{
	m_id = ECS::calculateId(m_index, m_version);
}

ECS::EntityId ECS::calculateId(Index index, Version version)
{
	return (static_cast<ECS::EntityId>(index) << INDEX_BITS) | static_cast<ECS::EntityId>(version);
}

ECS::Index ECS::toIndex(EntityId entityId)
{
	return entityId >> INDEX_BITS;
}

ECS::Version ECS::toVersion(EntityId entityId)
{
	return entityId & MAX_INDEX;
}