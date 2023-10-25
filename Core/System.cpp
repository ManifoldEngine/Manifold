#include "System.h"

using namespace ECSEngine;

std::string_view SystemBase::getName() const
{
    return "NONE";
}

float ECSEngine::SystemBase::getFrameTime() const
{
    return 0.0f;
}

bool ECSEngine::SystemBase::shouldTick(EntityRegistry& entityRegistry) const
{
    return false;
}

void ECSEngine::SystemBase::initialize(World* inPWorld, EntityRegistry& entityRegistry)
{
    pWorld = inPWorld;
    onInitialize(entityRegistry);
}

void ECSEngine::SystemBase::deinitialize(EntityRegistry& entityRegistry)
{
    onDeinitialize(entityRegistry);
}

World* SystemBase::getWorld() const
{
    return pWorld;
}

void SystemBase::tick(float deltaTime, EntityRegistry& entityRegistry)
{
}

void ECSEngine::SystemBase::onInitialize(EntityRegistry& entityRegistry)
{
}

void ECSEngine::SystemBase::onDeinitialize(EntityRegistry& entityRegistry)
{
}
