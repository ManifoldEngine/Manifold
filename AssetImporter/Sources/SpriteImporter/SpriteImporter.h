#pragma once

#include <filesystem>
#include <string>
#include <memory>

namespace Mani
{
    const std::string_view LogSpriteImporter = "SpriteImporter";

    struct Sprite;

    class SpriteImporter
    {
    public:
        static bool importFromPath(const std::filesystem::path& path, Sprite& outSprite, uint32_t defaultTPU = 512);
        static bool exportToPath(const std::filesystem::path& path, const Sprite& sprite);
    };
}
