#include <ManiTests/ManiTests.h>

#include <Core/ManiTests.h>
#include <Core/CoreFwd.h>
#include <Core/ManiTime.h>

#include <Animation/AnimationSystem.h>
#include <Animation/Animation.h>
#include <Animation/ShaderNames.h>

#include <Camera/Camera.h>

#include <RenderAPI/Components/MeshRendering.h>
#include <RenderAPI/Texture.h>

#include <Resources/Resources.h>
#include <Resources/ResourceSystem.h>

#include <filesystem>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	Mani::runTests();
}
#endif

using namespace Mani;

namespace AnimationTestsParams
{
	float gStubbedTimeDelta = .25f;
}

class TimeStubSystem : public Mani::ECS::System
{
public:
	virtual std::string_view getName() const override { return "TimeStubSystem"; }
	virtual bool shouldTick(const Mani::ECS::Registry& registry) const override { return true; }
	virtual ETickGroup getTickGroup() const override { return ETickGroup::PreUpdate; }

	virtual void tick(Mani::ECS::Registry& registry) override
	{
		Ref<Time> time = registry.getSingle<Time>();
		time->delta = AnimationTestsParams::gStubbedTimeDelta;
		time->elapsed += time->delta;
	}
};

MANI_SECTION_BEGIN(AnimationTests, "Animation")
{
	MANI_TEST(ShouldLoadAndPlayAnimationOneShot, "should load and play an animation one shot")
	{
		AnimationTestsParams::gStubbedTimeDelta = .25f * 0.5;
		Application app;
		World& world = app.getWorld(); 
		world.initialize();

		world.createSystem<AnimationSystem>();
		world.createSystem<TimeStubSystem>();

		ECS::Registry& registry = world.getMutableRegistry();
		Cameras::createMainCamera(registry);
		
		// animation loading
		const ECS::EntityId animationId = Resources::loadSync<Animation>(registry, "Engine/Modules/Animation/Tests/Assets/TestAnimation.json");
		MANI_TEST_ASSERT(Resources::isReady(registry, animationId), "Animation should be loaded and ready");
		const LoadedAnimation& animation = registry.getPinned<LoadedAnimation>(animationId);
		MANI_TEST_ASSERT(animation.frames.count() == 4, "4 frames should have been loaded");
		for (const auto& frame : animation.frames)
		{
			MANI_TEST_ASSERT(frame.textureId != ECS::INVALID_ID, "each frame should point to a loaded texture if any is set");
		}

		// animation play OneShot
		ECS::EntityId entityId = registry.create();

		{
			// setup
			auto [position, rotation, scale, animator, meshComponent, boundingSphere] = registry.addMany<Position, Rotation, Scale, Animator, MeshRendering, BoundingSphere>(entityId);
			boundingSphere->radius = 1.f;
			animator->playRate = 1.f / 4.f; // 4 fps
			Animations::play(registry, entityId, animationId);
		}

		// animation tick
		world.tick();

		{
			auto meshComponent = registry.get<MeshRendering>(entityId);
			const ECS::EntityId& textureId = meshComponent->textureParameters[Mani::ShaderNames::MANI_SPRITE_TEXTURE];
			MANI_TEST_ASSERT(textureId == animation.frames[0].textureId, "The first frame should be displayed");

		}

		// animation tick
		// tick is 1/8th of a second, so we need to tick twice to hit the next animation frame
		world.tick();
		world.tick();

		{
			auto meshComponent = registry.get<MeshRendering>(entityId);
			const ECS::EntityId& textureId = meshComponent->textureParameters[Mani::ShaderNames::MANI_SPRITE_TEXTURE];
			MANI_TEST_ASSERT(textureId == animation.frames[1].textureId, "The second frame should be displayed");
		}

		// animation tick
		// tick is 1/8th of a second, so we need to tick twice to hit the next animation frame
		world.tick();
		world.tick();

		{
			auto meshComponent = registry.get<MeshRendering>(entityId);
			const ECS::EntityId& textureId = meshComponent->textureParameters[Mani::ShaderNames::MANI_SPRITE_TEXTURE];
			MANI_TEST_ASSERT(textureId == animation.frames[2].textureId, "The third frame should be displayed");
		}

		// animation tick
		// tick is 1/8th of a second, so we need to tick twice to hit the next animation frame
		world.tick();
		world.tick();

		{
			auto meshComponent = registry.get<MeshRendering>(entityId);
			const ECS::EntityId& textureId = meshComponent->textureParameters[Mani::ShaderNames::MANI_SPRITE_TEXTURE];
			MANI_TEST_ASSERT(textureId == animation.frames[3].textureId, "The fourth frame should be displayed");
		}
		
		// animation tick
		// tick is 1/8th of a second, so we need to tick twice to hit the next animation frame
		world.tick();
		world.tick();

		{
			// One shot play mode should reset the animator and prameters once the animation is done
			auto meshComponent = registry.get<MeshRendering>(entityId);
			MANI_TEST_ASSERT(!meshComponent->textureParameters.has(Mani::ShaderNames::MANI_SPRITE_TEXTURE), "Anim texture should have been cleared");

			auto animator = registry.get<Animator>(entityId);
			MANI_TEST_ASSERT(animator->frameId == INVALID_FRAME_ID, "animator should not be playing anything");
			MANI_TEST_ASSERT(animator->resourceId == ECS::INVALID_ID, "animator should not be playing anything");
		}
	}

	MANI_TEST(ShouldLoadAndPlayAnimationLoop, "should load and play an animation loop")
	{
		AnimationTestsParams::gStubbedTimeDelta = .25f * 0.5;
		Application app;
		World& world = app.getWorld();
		world.initialize();

		world.createSystem<AnimationSystem>();
		world.createSystem<TimeStubSystem>();

		ECS::Registry& registry = world.getMutableRegistry();
		Cameras::createMainCamera(registry);

		// animation loading
		const ECS::EntityId animationId = Resources::loadSync<Animation>(registry, "Engine/Modules/Animation/Tests/Assets/TestAnimation.json");
		MANI_TEST_ASSERT(Resources::isReady(registry, animationId), "Animation should be loaded and ready");
		const LoadedAnimation& animation = registry.getPinned<LoadedAnimation>(animationId);
		MANI_TEST_ASSERT(animation.frames.count() == 4, "4 frames should have been loaded");
		for (const auto& frame : animation.frames)
		{
			MANI_TEST_ASSERT(frame.textureId != ECS::INVALID_ID, "each frame should point to a loaded texture if any is set");
		}

		// animation play OneShot
		ECS::EntityId entityId = registry.create();

		{
			// setup
			auto [position, rotation, scale, animator, meshComponent, boundingSphere] = registry.addMany<Position, Rotation, Scale, Animator, MeshRendering, BoundingSphere>(entityId);
			boundingSphere->radius = 1.f;
			animator->playRate = 1.f / 4.f; // 4 fps
			Animations::play(registry, entityId, animationId, Animator::EPlayMode::Loop);
		}

		// animation tick
		world.tick();

		{
			auto meshComponent = registry.get<MeshRendering>(entityId);
			const ECS::EntityId& textureId = meshComponent->textureParameters[Mani::ShaderNames::MANI_SPRITE_TEXTURE];
			MANI_TEST_ASSERT(textureId == animation.frames[0].textureId, "The first frame should be displayed");
		}

		// animation tick
		// tick is 1/8th of a second, so we need to tick twice to hit the next animation frame
		for (int i = 0; i < 6; i++)
		{
			world.tick();
		}

		{
			auto meshComponent = registry.get<MeshRendering>(entityId);
			const ECS::EntityId& textureId = meshComponent->textureParameters[Mani::ShaderNames::MANI_SPRITE_TEXTURE];
			MANI_TEST_ASSERT(textureId == animation.frames[3].textureId, "The fourth frame should be displayed");
		}

		// animation tick
		// tick is 1/8th of a second, so we need to tick twice to hit the next animation frame
		world.tick();
		world.tick();

		{
			// should loop
			auto meshComponent = registry.get<MeshRendering>(entityId);
			const ECS::EntityId& textureId = meshComponent->textureParameters[Mani::ShaderNames::MANI_SPRITE_TEXTURE];
			MANI_TEST_ASSERT(textureId == animation.frames[0].textureId, "The first frame should be displayed");
		}
	}
}
MANI_SECTION_END(CameraTests)