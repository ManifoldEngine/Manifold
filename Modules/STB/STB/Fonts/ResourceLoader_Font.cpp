#include "ResourceLoader_font.h"

#include <RenderAPI/Font.h>
#include <STB/Fonts/STBTruetype.h>

using namespace Mani;

bool ResourceLoader_Font::load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
	Resource<Font>& res = registry.getPinned<Resource<Font>>(resourceId);
	return STB::loadFont(absolutePath, res.value);
}

bool ResourceLoader_Font::unload(ECS::Registry& registry, ECS::EntityId resourceId) const
{
	Resource<Font>& res = registry.getPinned<Resource<Font>>(resourceId);
	return STB::unloadFont(res.value);
}
