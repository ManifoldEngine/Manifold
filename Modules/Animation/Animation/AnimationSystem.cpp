#include "AnimationSystem.h"

#include <Core/TimeSystem.h>
#include <Core/Debug/Profiling.h>
#include <Core/Async/Parallel.h>

#include <Camera/Camera.h>
#include <Camera/CameraSystem.h>

#include <Animation/Animation.h>
#include <Animation/ShaderNames.h>

#include <RenderAPI/Components/MeshRendering.h>
#include <RenderAPI/Texture.h>
#include <RenderAPI/Shader.h>

#include <Resources/ResourceSystem.h>
#include <Resources/Resources.h>

using namespace Mani;

void updateEntity(ECS::Registry& registry, ECS::EntityId entityId, const LoadedAnimation& animation, FrameId frameId)
{
	MANI_ASSERT(frameId <= animation.frames.count(), "frame id out of bounds");
	const LoadedAnimation::Frame& frame = animation.frames[frameId];

	if (Ref<MeshRendering> meshComponent = registry.find<MeshRendering>(entityId))
	{
		meshComponent->textureParameters[ShaderNames::MANI_TEXTURE_0] = frame.textureId;
	}
}

void resetEntity(ECS::Registry& registry, ECS::EntityId entityId)
{
	if (Ref<MeshRendering> meshComponent = registry.find<MeshRendering>(entityId))
	{
		meshComponent->textureParameters.remove(ShaderNames::MANI_TEXTURE_0);
	}
}

void AnimationSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();
	world.initializeDependency<CameraSystem>();
	world.initializeDependency<ResourceSystem>();

	Resources::registerExtension(registry, &extension);
}

void AnimationSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterExtension(registry, &extension);
}

void AnimationSystem::tick(ECS::Registry& registry)
{
	ECS::EntityId cameraId = CameraStatics::getMainCameraId(registry);
	MANI_ASSERT(cameraId != ECS::INVALID_ID, "trying to animate without a camera");
	Ref<Camera> camera = registry.get<Camera>(cameraId);

	Ref<Time> time = registry.getSingle<Time>();
	ECS::View<Animator, Position, Rotation, Scale, BoundingSphere> view(registry);
	parallelFor(view, [&registry, &time, &camera](ECS::EntityId entityId, Animator& animator, Position& position, Rotation& rotation, Scale& scale, BoundingSphere& bounds)
	{
		if (Math::isEqual(animator.playRate, 0.f) || animator.resourceId == ECS::INVALID_ID)
		{
			return;
		}

		if (animator.shouldCull && !CameraStatics::isInView(*camera, position, rotation, scale, bounds))
		{
			return;
		}
		
		if (!Resources::isReady(registry, animator.resourceId))
		{
			return;
		}

		const LoadedAnimation& animation = registry.getPinned<LoadedAnimation>(animator.resourceId);
		if (animator.frameId == INVALID_FRAME_ID)
		{
			// play the first frame.
			animator.frameId = 0;
			updateEntity(registry, entityId, animation, animator.frameId);
			return;
		}

		animator.elapsed += time->delta;
		if (animator.elapsed < animator.playRate)
		{
			return;
		}

		const FrameId frameDelta = Math::floorToInt(animator.elapsed / animator.playRate);
		animator.frameId += frameDelta;
		const float frameDeltaInSeconds = animator.playRate * frameDelta;
		const float elapsedRemainder = animator.elapsed - frameDeltaInSeconds;

		if (animator.frameId >= animation.frames.count())
		{
			switch (animator.mode)
			{
				case Animator::EPlayMode::Loop:
				{
					animator.frameId = 0;
					break;
				}

				case Animator::EPlayMode::OneShot:
				default:
				{
					animator.frameId = INVALID_FRAME_ID;
					animator.resourceId = ECS::INVALID_ID;
					break;
				}
			}
		}
		
		if (animator.frameId != INVALID_FRAME_ID)
		{
			updateEntity(registry, entityId, animation, animator.frameId);
		}
		else
		{
			resetEntity(registry, entityId);
		}

		animator.elapsed = elapsedRemainder;
	});
}

void AnimationResourceSystemExtension::onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	if (!registry.hasPinned<Resource<Animation>>(entityId))
	{
		return;
	}

	const Animation& animation = registry.getPinned<Resource<Animation>>(entityId).value;
	LoadedAnimation& loadedAnimation = registry.addPinned<LoadedAnimation>(entityId);
	for (const auto& frame : animation.frames)
	{
		loadedAnimation.frames.add(LoadedAnimation::Frame{ .textureId = Resources::load<Texture>(registry, frame.texturePath, tag) });
	}
}

void AnimationResourceSystemExtension::onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	if (!registry.hasPinned<Resource<Animation>>(entityId))
	{
		return;
	}

	const LoadedAnimation& animation = registry.getPinned<LoadedAnimation>(entityId);
	for (auto& frame : animation.frames)
	{
		Resources::unload(registry, frame.textureId);
	}
}
