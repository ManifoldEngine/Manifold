#include "System.h"
#include <ECS/Registry.h>
#include <Log.h>

using namespace Mani;

std::string_view SystemBase::getName() const
{
    return "NONE";
}

bool SystemBase::shouldTick(ECS::Registry& registry) const
{
    return false;
}

ETickGroup SystemBase::getTickGroup() const
{
    return ETickGroup::Tick;
}

void SystemBase::initialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
    if (m_isInitialized)
    {
        return;
    }

    MANI_LOG(LogCore, "Initialized {}", getName());
    onInitialize(registry, systemContainer);
    m_isInitialized = true;
}

void SystemBase::deinitialize(ECS::Registry& registry)
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

void SystemBase::tick(float deltaTime, ECS::Registry& registry)
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

void SystemBase::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
}

void SystemBase::onDeinitialize(ECS::Registry& registry)
{
}

void SystemBase::onEnabled()
{
}

void SystemBase::onDisabled()
{
}
