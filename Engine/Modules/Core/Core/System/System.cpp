#include "System.h"
#include <Log.h>

using namespace Mani;

std::string_view SystemBase::getName() const
{
    return "NONE";
}

bool SystemBase::shouldTick(EntityRegistry& registry) const
{
    return false;
}

ETickGroup SystemBase::getTickGroup() const
{
    return ETickGroup::Tick;
}

void SystemBase::initialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    if (m_isInitialized)
    {
        return;
    }

    MANI_LOG(LogCore, "Initialized {}", getName());
    onInitialize(registry, systemContainer);
    m_isInitialized = true;
}

void SystemBase::deinitialize(EntityRegistry& registry)
{
    if (!m_isInitialized)
    {
        return;
    }

    onDeinitialize(registry);
    m_isInitialized = false;
    MANI_LOG(LogCore, "Deinitialized {}", getName());
}

void SystemBase::enable()
{
    if (m_isEnabled)
    {
        return;
    }

    m_isEnabled = true;
    onEnabled();
}

void SystemBase::disable()
{
    if (!m_isEnabled)
    {
        return;
    }

    m_isEnabled = false;
    onDisabled();
}

void SystemBase::tick(float deltaTime, EntityRegistry& registry)
{
}

bool SystemBase::isInitialized() const
{
    return m_isInitialized;
}

bool Mani::SystemBase::isEnabled() const
{
    return m_isEnabled;
}

void SystemBase::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
}

void SystemBase::onDeinitialize(EntityRegistry& registry)
{
}

void SystemBase::onEnabled()
{
}

void SystemBase::onDisabled()
{
}
