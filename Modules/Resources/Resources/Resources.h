#pragma once

#include <string_view>
#include <limits>

namespace Mani
{
    constexpr std::string_view LogResources = "Resources";
    constexpr uint32_t GLOBAL_RESOURCE_TAG = (std::numeric_limits<uint32_t>::max)();
}