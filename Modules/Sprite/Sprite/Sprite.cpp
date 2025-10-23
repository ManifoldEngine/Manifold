#include "Sprite.h"

#include <Core/Async/Parallel.h>

#include <Resources/ResourceSystem.h>

#include <RenderAPI/BoundingSphere.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Mesh.h>
#include <RenderAPI/MeshRendering.h>
#include <RenderAPI/Primitives.h>
#include <RenderAPI/Texture.h>

#include <Sprite/ResourceLoader_Sprite.h>

using namespace Mani;

void onSpriteLoaded(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId spriteId, const std::string_view& shaderPath, uint32_t tag)
{
	MeshRendering& meshRendering = registry.getRef<MeshRendering>(entityId);
	const Resource<Sprite>& spriteRes = registry.getRef<Resource<Sprite>>(spriteId);
	MANI_ASSERT(spriteRes.isReady, "Texture isn't ready yet.");

	const Sprite& sprite = spriteRes.value;

	const std::string texturePathString = std::string(sprite.texturePath);
	Material material
	{
		.name = texturePathString,
		.shaderPath = std::string(shaderPath),
		.textures = {{ Mani::ShaderNames::MANI_TEXTURE_0, texturePathString }},
	};

	meshRendering.meshHandle = sprite.quadId;
	meshRendering.materialHandle = ResourceSystem::injectResource<Material>(registry, std::move(material), tag);

	const Texture& texture = registry.getRef<Resource<Texture>>(sprite.textureId).value;
	BoundingSphere& bounds = *registry.add<BoundingSphere>(entityId);
	const float sizeX = static_cast<float>(texture.size.x / sprite.texelsPerUnit);
	const float sizeY = static_cast<float>(texture.size.y / sprite.texelsPerUnit);
	bounds.radius = Math::sqrt(sizeX * sizeX + sizeY * sizeY);
}

void waitForSpriteResourceAsync(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId spriteId, const std::string_view& shaderPath, uint32_t tag)
{
	Mani::enqueueTask([&] {
		const Resource<Sprite>& spriteRes = registry.getRef<Resource<Sprite>>(spriteId);
		while (!spriteRes.isReady)
		{
			std::this_thread::yield();
		}

		// we defer to the end of frame to avoir race read/write conflict
		Mani::defer([&] {
			onSpriteLoaded(registry, entityId, spriteId, shaderPath, tag);
		});
	});
}

ECS::EntityId SpriteStatics::getOrAddQuad(ECS::Registry& registry, const Vec2i& size)
{
	SpriteQuadDatabase* database = registry.getSingle<SpriteQuadDatabase>();
	MANI_ASSERT(database != nullptr, "Trying to get or add sprite quad without the SpriteSystem being initialized");
	if (const auto* quadId = database->quads.find(size))
	{
		return *quadId;
	}

	const Vec2f sizef{
		.x = static_cast<float>(size.x),
		.y = static_cast<float>(size.y),
	};
	ECS::EntityId quadId = ResourceSystem::injectResource<Mesh>(registry, Primitives::makeQuad(sizef), Mani::GLOBAL_RESOURCE_TAG);
	database->quads.add(size, quadId);
	return quadId;
}

void SpriteStatics::loadAsyncAndAddSprite(ECS::Registry& registry, ECS::EntityId entityId, const std::string_view& spritePath, const std::string_view& shaderPath, uint32_t tag)
{
	MANI_ASSERT(registry.has<MeshRendering>(entityId), "Trying to add a sprite to an entity that doesn't have any mesh rendering");

	const ECS::EntityId spriteId = ResourceSystem::loadResource<Sprite>(registry, spritePath, tag);
	const Resource<Sprite>& spriteRes = registry.getRef<Resource<Sprite>>(spriteId);
	if (!spriteRes.isReady)
	{
		waitForSpriteResourceAsync(registry, entityId, spriteId, shaderPath, tag);
		return;
	}

	onSpriteLoaded(registry, entityId, spriteId, shaderPath, tag);
}
