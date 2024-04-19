#include "ConfigSystem.h"
#include <nlohmann/json.hpp>

using namespace Mani;

std::string_view ConfigSystem::getName() const
{
    return "ConfigSystem";
}

bool ConfigSystem::shouldTick(EntityRegistry& registry) const
{
    return false;
}
