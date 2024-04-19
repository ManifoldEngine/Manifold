#pragma once

#include <string>

namespace Mani
{
    class IJsonAsset
    {
    public:
        virtual void parse(const std::string_view& content) = 0;
        virtual std::string toJson() = 0;
    };
}