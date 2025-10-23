#include <ManiTests/ManiTests.h>

#include <Core/CoreFwd.h>
#include <Core/ManiTests.h>

#include <Resources/ResourceSystem.h>

#include <Sprite/Sprite.h>
#include <Sprite/SpriteSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Texture.h>
#include <RenderAPI/Primitives.h>
#include <RenderAPI/MeshRendering.h>

#include <OpenGL/ResourceLoader_Texture.h>
#include <Sprite/ResourceLoader_Sprite.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	Mani::runTests();
}
#endif

using namespace Mani;

MANI_SECTION_BEGIN(SpriteTests, "Sprites")
{
	MANI_TEST(ShouldLoadASprite, "Should load a sprite")
	{
		World world;
		world.initialize();
		world.createSystem<SpriteSystem>()
			 .createSystem<ResourceSystem>();

		ECS::Registry& registry = world.getMutableRegistry();
		
		ECS::EntityId spriteId = ResourceSystem::loadResourceSync<Sprite>(registry, "Engine/Modules/Sprite/Tests/Assets/blue_square.sprite");
		const Resource<Sprite>& spriteRes = registry.getRef<Resource<Sprite>>(spriteId);
		MANI_TEST_ASSERT(spriteRes.isReady, "Sprite resource should be ready");

		const Resource<Texture>& textureRes = registry.getRef<Resource<Texture>>(spriteRes.value.textureId);
		MANI_TEST_ASSERT(textureRes.isReady, "Texture should be ready");

		const Resource<Mesh>& meshRes = registry.getRef<Resource<Mesh>>(spriteRes.value.quadId);
		MANI_TEST_ASSERT(meshRes.isReady, "Texture should be ready");
	}

	MANI_TEST(ShouldAddASpriteToAMeshRendering, "Should setup a mesh rendering for sprite rendering")
	{
		World world;
		world.initialize();
		world.createSystem<SpriteSystem>()
			 .createSystem<ResourceSystem>();

		ECS::Registry& registry = world.getMutableRegistry();

		ECS::EntityId spriteId = ResourceSystem::loadResourceSync<Sprite>(registry, "Engine/Modules/Sprite/Tests/Assets/blue_square.sprite");
		const Resource<Sprite>& spriteRes = registry.getRef<Resource<Sprite>>(spriteId);
		MANI_TEST_ASSERT(spriteRes.isReady, "Sprite resource should be ready");

		ECS::EntityId entityId = registry.create();
		registry.add<MeshRendering>(entityId);
		SpriteStatics::loadAsyncAndAddSprite(registry, entityId, "Engine/Modules/Sprite/Tests/Assets/blue_square.sprite", "some/shader/path.shader", Mani::GLOBAL_RESOURCE_TAG);
		MeshRendering& meshRendering = registry.getRef<MeshRendering>(entityId);
		MANI_TEST_ASSERT(registry.isValid(meshRendering.meshHandle), "mesh handle should be valid");
		MANI_TEST_ASSERT(registry.isValid(meshRendering.materialHandle), "material handle should be valid");
	}
}
MANI_SECTION_END(SpriteTests)