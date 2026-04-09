#include <ManiTests/ManiTests.h>

#include <Core/CoreFwd.h>
#include <Core/ManiTests.h>

#include <Resources/ResourceSystem.h>
#include <Resources/IResourceLoader.h>
#include <Resources/Resources.h>

#include <Sprite/Sprite.h>
#include <Sprite/SpriteSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Texture.h>
#include <RenderAPI/Primitives.h>
#include <RenderAPI/MeshRendering.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	Mani::runTests();
}
#endif

using namespace Mani;

class StubResourceLoader_Texture : public IResourceLoader
{
	virtual bool load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const override
	{
		Ref<Resource<Texture>> resource = registry.get<Resource<Texture>>(resourceId);
		resource->value.size = { 32, 32 };
		return true;
	}
};

MANI_SECTION_BEGIN(SpriteTests, "Sprites")
{
	MANI_TEST(ShouldLoadASprite, "Should load a sprite")
	{
		World world;
		world.initialize();
		world.createSystem<ResourceSystem>()
			.createSystem<SpriteSystem>();

		ECS::Registry& registry = world.getMutableRegistry();
		
		StubResourceLoader_Texture loader;
		Resources::registerLoaderFor<Texture>(registry, &loader);
		
		ECS::EntityId spriteId = Resources::loadSync<Sprite>(registry, "Engine/Modules/Sprite/Tests/Assets/blue_square.sprite");
		Ref<Resource<Sprite>> sprite = registry.get<Resource<Sprite>>(spriteId); // should not asset
		MANI_TEST_ASSERT(Resources::isReady(registry, spriteId), "Sprite resource should be ready");

		Ref<Resource<Texture>> texture = registry.get<Resource<Texture>>(sprite->value.textureId); // should not asset
		MANI_TEST_ASSERT(Resources::isReady(registry, sprite->value.textureId), "Texture should be ready");

		Ref<Resource<Mesh>> mesh = registry.get<Resource<Mesh>>(sprite->value.quadId); // should not asset
		MANI_TEST_ASSERT(Resources::isReady(registry, sprite->value.quadId), "Texture should be ready");
	}

	MANI_TEST(ShouldAddASpriteToAMeshRendering, "Should setup a mesh rendering for sprite rendering")
	{
		World world;
		world.initialize();
		world.createSystem<SpriteSystem>()
			 .createSystem<ResourceSystem>();

		ECS::Registry& registry = world.getMutableRegistry();

		StubResourceLoader_Texture loader;
		Resources::registerLoaderFor<Texture>(registry, &loader);

		ECS::EntityId spriteId = Resources::loadSync<Sprite>(registry, "Engine/Modules/Sprite/Tests/Assets/blue_square.sprite");
		Ref<Resource<Sprite>> sprite = registry.get<Resource<Sprite>>(spriteId); // should not assert
		MANI_TEST_ASSERT(Resources::isReady(registry, spriteId), "Sprite resource should be ready");

		ECS::EntityId entityId = registry.create();
		constexpr Vec2i size{ 1, 1 };
		constexpr uint32_t TPU = 1;
		SpriteStatics::addSprite(registry, entityId, size, TPU, "Engine/Modules/Sprites/Tests/Assets/blue_square.sprite", "some/shader/path.shader", Mani::GLOBAL_RESOURCE_TAG);
		Ref<MeshRendering> meshRendering = registry.get<MeshRendering>(entityId);
		MANI_TEST_ASSERT(registry.isValid(meshRendering->meshResourceId), "mesh handle should be valid");
		MANI_TEST_ASSERT(registry.isValid(meshRendering->materialResourceId), "material handle should be valid");
	}
}
MANI_SECTION_END(SpriteTests)