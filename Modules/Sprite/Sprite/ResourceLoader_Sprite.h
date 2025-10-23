#pragma once

#include <Core/CoreFwd.h>

#include <Resources/Resource.h>
#include <Resources/ResourceSystem.h>

#include <RenderAPI/Texture.h>

#include <Sprite/Sprite.h>

#include <ManiZ/Json.h>

namespace Mani
{
	template<>
	bool ResourceLoader::load<Sprite>(ECS::Registry& registry, const std::filesystem::path& absolutePath, Resource<Sprite>& resource, uint32_t tag)
	{
		std::string content;
		if (!FileSystem::readFile(absolutePath, content))
		{
			MANI_LOG_ERROR(LogResources, "Could not find asset at path {}", absolutePath.string());
			return false;
		}

		resource.value = ManiZ::from::json<Sprite>(std::move(content));

		const std::string& texturePath = resource.value.texturePath;
		MANI_ASSERT(!texturePath.empty(), "Sprite asset with an empty texture path");
		resource.value.textureId = ResourceSystem::loadResourceSync<Texture>(registry, texturePath, tag);

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
}