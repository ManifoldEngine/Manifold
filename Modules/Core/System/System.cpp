#include "System.h"

using namespace ECSEngine;

std::string_view SystemBase::getName() const
{
    return "NONE";
}

float SystemBase::getFrameTime() const
{
    return 0.0f;
}

bool SystemBase::shouldTick(EntityRegistry& entityRegistry) const
{
    return false;
}

void SystemBase::initialize(EntityRegistry& entityRegistry, SystemContainer& systemContainer)
{
    if (m_bIsInitialized)
    {
        return;
    }

    onInitialize(entityRegistry, systemContainer);
    m_bIsInitialized = true;
}

void SystemBase::deinitialize(EntityRegistry& entityRegistry)
{
    if (!m_bIsInitialized)
    {
        return;
    }

    onDeinitialize(entityRegistry);
    m_bIsInitialized = false;
}

void SystemBase::tick(float deltaTime, EntityRegistry& entityRegistry)
{
}

void SystemBase::onInitialize(EntityRegistry& entityRegistry, SystemContainer& systemContainer)
{
}

void SystemBase::onDeinitialize(EntityRegistry& entityRegistry)
{
}
