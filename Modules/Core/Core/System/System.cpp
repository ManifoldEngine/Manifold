#include "System.h"
#include <Core/World/World.h>
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
    return ETickGroup::Update;
}

void SystemBase::initialize(ECS::Registry& registry, World& world)
{
    if (m_isInitialized)
    {
        return;
    }

    MANI_LOG(LogCore, "Initialized {}", getName());
    onInitialize(registry, world);
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

void SystemBase::tick(float deltaTime, ECS::Registry& registry)
{
}

bool SystemBase::isInitialized() const
{
    return m_isInitialized;
}

void SystemBase::onInitialize(ECS::Registry& registry, World& world)
{
}

void SystemBase::onDeinitialize(ECS::Registry& registry)
{
}
