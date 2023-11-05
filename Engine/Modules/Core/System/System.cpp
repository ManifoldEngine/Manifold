#include "System.h"
#include <Log.h>

using namespace ECSEngine;

std::string_view SystemBase::getName() const
{
    return "NONE";
}

float SystemBase::getFrameTime() const
{
    return 0.0f;
}

bool SystemBase::shouldTick(EntityRegistry& registry) const
{
    return false;
}

void SystemBase::initialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    if (m_isInitialized)
    {
        return;
    }

    onInitialize(registry, systemContainer);
    m_isInitialized = true;
    ECSE_LOG(LogCore, "Initialized {}", getName());
}

void SystemBase::deinitialize(EntityRegistry& registry)
{
    if (!m_isInitialized)
    {
        return;
    }

    onDeinitialize(registry);
    m_isInitialized = false;
    ECSE_LOG(LogCore, "Deinitialized {}", getName());
}

void SystemBase::tick(float deltaTime, EntityRegistry& registry)
{
}

bool SystemBase::isInitialized() const
{
    return m_isInitialized;
}

void SystemBase::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
}

void SystemBase::onDeinitialize(EntityRegistry& registry)
{
}
