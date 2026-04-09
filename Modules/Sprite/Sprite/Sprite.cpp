#include "Sprite.h"

#include <Core/Async/Parallel.h>

#include <Resources/Resources.h>

#include <RenderAPI/BoundingSphere.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Mesh.h>
#include <RenderAPI/MeshRendering.h>
#include <RenderAPI/Primitives.h>
#include <RenderAPI/Texture.h>

using namespace Mani;

void onSpriteLoaded(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId spriteId, const std::string_view& shaderPath, uint32_t tag, bool withBoundingSphere)
{
	Ref<MeshRendering> meshRendering = registry.get<MeshRendering>(entityId);
	MANI_ASSERT(Resources::isReady(registry, spriteId), "Texture isn't ready yet.");
	Ref<Resource<Sprite>> sprite = registry.get<Resource<Sprite>>(spriteId);

	const std::string texturePathString = std::string(sprite->value.texturePath);
	Material material
	{
		.name = texturePathString,
		.shaderPath = std::string(shaderPath),
		.textures = {{ Mani::ShaderNames::MANI_TEXTURE_0, texturePathString }},
	};

	meshRendering->meshResourceId = sprite->value.quadId;
	meshRendering->materialResourceId = Resources::inject<Material>(registry, std::move(material), tag);

	if (withBoundingSphere)
	{
		Ref<BoundingSphere> bounds = registry.add<BoundingSphere>(entityId);
		Ref<Resource<Texture>> texture = registry.get<Resource<Texture>>(sprite->value.textureId);
		const float sizeX = static_cast<float>(texture->value.size.x / sprite->value.texelsPerUnit);
		const float sizeY = static_cast<float>(texture->value.size.y / sprite->value.texelsPerUnit);
		bounds->radius = Math::sqrt(sizeX * sizeX + sizeY * sizeY);
	}
}

void waitForSpriteResourceAsync(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId spriteId, const std::string_view& shaderPath, uint32_t tag, bool withBoundingSphere)
{
	Mani::enqueueTask([=, &registry = registry, &shaderPath = shaderPath] {
		while (!Resources::isReady(registry, spriteId))
		{
			std::this_thread::yield();
		}

		// we defer to the end of frame to avoir race read/write conflict
		Mani::defer([=, &registry = registry, &shaderPath = shaderPath] {
			onSpriteLoaded(registry, entityId, spriteId, shaderPath, tag, withBoundingSphere);
		});
	});
}

ECS::EntityId SpriteStatics::getOrAddQuad(ECS::Registry& registry, const Vec2i& size)
{
	Ref<SpriteQuadDatabase> database = registry.findSingle<SpriteQuadDatabase>();
	MANI_ASSERT(database.isValid(), "Trying to get or add sprite quad without the SpriteSystem being initialized");
	if (const auto* quadId = database->quads.find(size))
	{
		return *quadId;
	}

	const Vec2f sizef{
		.x = static_cast<float>(size.x),
		.y = static_cast<float>(size.y),
	};
	ECS::EntityId quadId = Resources::inject<Mesh>(registry, Primitives::makeQuad(sizef), Mani::GLOBAL_RESOURCE_TAG);
	database = registry.getSingle<SpriteQuadDatabase>();
	database->quads.add(size, quadId);
	return quadId;
}

void SpriteStatics::addSprite(ECS::Registry& registry, ECS::EntityId entityId, const Vec2i& size, uint32_t texelsPerUnits, const std::string_view& texturePath, const std::string_view& shaderPath, uint32_t tag, bool withBoundingSphere)
{
	const std::string texturePathString = std::string(texturePath);
	Material material
	{
		.name = texturePathString,
		.shaderPath = std::string(shaderPath),
		.textures = {{ Mani::ShaderNames::MANI_TEXTURE_0, texturePathString }},
	};

	Ref<MeshRendering> rendering = registry.add<MeshRendering>(entityId);
	rendering->meshResourceId = SpriteStatics::getOrAddQuad(registry, size);
	rendering->materialResourceId = Resources::inject<Material>(registry, std::move(material), tag);

	if (withBoundingSphere)
	{
		const float sizeX = static_cast<float>(size.x);
		const float sizeY = static_cast<float>(size.y);
		registry.add<BoundingSphere>(entityId, Math::sqrt(sizeX * sizeX + sizeY * sizeY));
	}
}

void SpriteStatics::loadAsyncAndAddSprite(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& spritePath, const std::string_view& shaderPath, uint32_t tag, bool withBoundingSphere)
{
	MANI_ASSERT(registry.has<MeshRendering>(entityId), "Trying to add a sprite to an entity that doesn't have any mesh rendering");

	const ECS::EntityId spriteId = Resources::load<Sprite>(registry, spritePath, tag);
	if (!Resources::isReady(registry, spriteId))
	{
		waitForSpriteResourceAsync(registry, entityId, spriteId, shaderPath, tag, withBoundingSphere);
		return;
	}

	onSpriteLoaded(registry, entityId, spriteId, shaderPath, tag, withBoundingSphere);
}