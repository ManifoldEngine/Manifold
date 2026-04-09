#include "ResourceLoader_Sprite.h"

#include <Core/Async/Defer.h>
#include <Core/FileSystem.h>

#include <Resources/Resources.h>

#include <RenderAPI/Texture.h>
#include <Sprite/Sprite.h>
#include <ManiZ/Json.h>

using namespace Mani;

bool ResourceLoader_Sprite::load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
	Ref<Resource<Sprite>> resource = registry.get<Resource<Sprite>>(resourceId);

	std::string content;
	if (!FileSystem::readFile(absolutePath, content))
	{
		MANI_LOG_ERROR(LogSprite, "Could not find asset at path {}", absolutePath.string());
		return false;
	}

	resource->value = ManiZ::from::json<Sprite>(std::move(content));

	const std::string& texturePath = resource->value.texturePath;
	MANI_ASSERT(!texturePath.empty(), "Sprite asset with an empty texture path");

	const ECS::EntityId textureId = Resources::loadSync<Texture>(registry, texturePath, tag);
	resource = registry.get<Resource<Sprite>>(resourceId); // refresh the reference, we added a texture component
	resource->value.textureId = textureId;

	Ref<Resource<Texture>> texture = registry.get<Resource<Texture>>(resource->value.textureId);
	const float texelsPerUnitf = static_cast<float>(resource->value.texelsPerUnit);
	MANI_ASSERT(texelsPerUnitf > 0.f, "tpu of 0 doesn't make sense");

	const Vec2i size{
		.x = Math::ceilToInt(static_cast<float>(texture->value.size.x) / texelsPerUnitf),
		.y = Math::ceilToInt(static_cast<float>(texture->value.size.y) / texelsPerUnitf),
	};

	const ECS::EntityId quadId = SpriteStatics::getOrAddQuad(registry, size);
	resource = registry.get<Resource<Sprite>>(resourceId);
	resource->value.quadId = quadId;
	return true;
}

void ResourceLoader_Sprite::postLoad(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
}
