#include "ResourceLoader_Sprite.h"

#include <Core/Async/Defer.h>
#include <Core/FileSystem.h>

#include <Resources/Resources.h>

#include <RenderAPI/Texture.h>
#include <Sprite/Sprite.h>
#include <ManiZ/Json.h>

using namespace Mani;

ECS::ComponentId ResourceLoader_Sprite::getComponentId(const ECS::Registry& registry) const
{
	return registry.getComponentId<Resource<Sprite>>();
}

bool ResourceLoader_Sprite::load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
	Resource<Sprite>& resource = registry.getRef<Resource<Sprite>>(resourceId);

	std::string content;
	if (!FileSystem::readFile(absolutePath, content))
	{
		MANI_LOG_ERROR(LogSprite, "Could not find asset at path {}", absolutePath.string());
		return false;
	}

	resource.value = ManiZ::from::json<Sprite>(std::move(content));

	const std::string& texturePath = resource.value.texturePath;
	MANI_ASSERT(!texturePath.empty(), "Sprite asset with an empty texture path");
	resource.value.textureId = Resources::loadSync<Texture>(registry, texturePath, tag);

	const Texture& texture = registry.getRef<Resource<Texture>>(resource.value.textureId).value;
	const float texelsPerUnitf = static_cast<float>(resource.value.texelsPerUnit);
	MANI_ASSERT(texelsPerUnitf > 0.f, "tpu of 0 doesn't make sense");

	const Vec2i size{
		.x = Math::ceilToInt(static_cast<float>(texture.size.x) / texelsPerUnitf),
		.y = Math::ceilToInt(static_cast<float>(texture.size.y) / texelsPerUnitf),
	};

	resource.value.quadId = SpriteStatics::getOrAddQuad(registry, size);
	return true;
}
