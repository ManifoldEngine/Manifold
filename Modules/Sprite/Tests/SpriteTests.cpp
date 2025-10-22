#include <ManiTests/ManiTests.h>

#include <Core/CoreFwd.h>
#include <Core/ManiTests.h>

#include <Resources/ResourceSystem.h>

#include <Sprite/Sprite.h>
#include <Sprite/SpriteSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Texture.h>
#include <RenderAPI/Primitives.h>

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
}
MANI_SECTION_END(SpriteTests)