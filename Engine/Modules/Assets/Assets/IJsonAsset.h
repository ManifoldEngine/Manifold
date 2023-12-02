#pragma once

#include <string>

namespace ECSEngine
{
    class IJsonAsset
    {
        virtual void parse(const std::string_view& content) = 0;
        virtual std::string toJson() = 0;
    };
}