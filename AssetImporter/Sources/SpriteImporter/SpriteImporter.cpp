#include "SpriteImporter.h"

#include <Core/CoreFwd.h>
#include <Core/FileSystem.h>

#include <Sprite/Sprite.h>
#include <ManiZ/ManiZ.h>
#include <filesystem>

using namespace Mani;
namespace fs = std::filesystem;

bool SpriteImporter::importFromPath(const fs::path& path, std::shared_ptr<Sprite>& outSprite, uint32_t defaultTPU)
{
	MANI_ASSERT(outSprite != nullptr, "provided shader cannot be null");
	const std::string filename = std::filesystem::path(path).stem().string();
	const fs::path relatedSpritePath = path.parent_path().append(filename + ".sprite");
	if (fs::exists(relatedSpritePath))
	{
		return false;
	}
	
	outSprite->texturePath = path.string();
	outSprite->texelsPerUnit = defaultTPU;
	return true;
}

bool SpriteImporter::exportToPath(const fs::path& path, const std::shared_ptr<Sprite>& sprite)
{
	MANI_ASSERT(sprite != nullptr, "provided shader cannot be null");
	return FileSystem::writeFile(path, ManiZ::to::json(*sprite));
}
