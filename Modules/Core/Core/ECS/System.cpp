#include "System.h"
#include <Core/World.h>
#include <Core/ECS/Registry.h>
#include <Log.h>

using namespace Mani;

std::string_view ECS::System::getName() const
{
    return "NONE";
}

bool ECS::System::shouldTick(ECS::Registry& registry) const
{
    return false;
}

ETickGroup ECS::System::getTickGroup() const
{
    return ETickGroup::Update;
}

void ECS::System::initialize(ECS::Registry& registry, World& world)
{
    if (m_isInitialized)
    {
        return;
    }

    MANI_LOG(LogCore, "Initialized {}", getName());
    m_isInitialized = true;
    onInitialize(registry, world);
}

void ECS::System::deinitialize(ECS::Registry& registry, World& world)
{
    if (!m_isInitialized)
    {
        return;
    }

    onDeinitialize(registry, world);
    m_isInitialized = false;
    MANI_LOG(LogCore, "Deinitialized {}", getName());
}

void ECS::System::tick(ECS::Registry& registry)
{
}

bool ECS::System::isInitialized() const
{
    return m_isInitialized;
}

void ECS::System::onInitialize(ECS::Registry& registry, World& world)
{
}

void ECS::System::onDeinitialize(ECS::Registry& registry, World& world)
{
}
