#include "AnimationSystem.h"

#include <Core/TimeSystem.h>
#include <Core/Debug/Profiling.h>
#include <Core/Async/Parallel.h>

#include <Camera/CameraSystem.h>

#include <Animation/Animation.h>

#include <RenderAPI/MeshComponent.h>
#include <Resources/Resource.h>

using namespace Mani;

void updateEntity(ECS::Registry& registry, ECS::EntityId entityId, const Animation& animation, FrameId frameId)
{
	MANI_ASSERT(frameId <= animation.frames.size(), "frame id out of bounds");
	const Animation::Frame& frame = animation.frames[frameId];

	if (MeshComponent* meshComponent = registry.get<MeshComponent>(entityId))
	{

	}
}

void AnimationSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();
	world.initializeDependency<CameraSystem>();
}

void AnimationSystem::tick(ECS::Registry& registry)
{
	MANI_TIME_SCOPE(AnimationSystem_tick);

	ECS::EntityId cameraId = CameraStatics::getMainCameraId(registry);
	MANI_ASSERT(cameraId != ECS::INVALID_ID, "trying to animate without a camera");
	Camera& camera = *registry.get<Camera>(cameraId);

	const Time& time = *registry.getSingle<Time>();
	ECS::View<Animator, Position, Rotation, Scale, BoundingSphere> view(registry);
	parallelFor(view, [&registry, &time, &camera](const auto entityId, size_t threadIndex)
	{
		auto [animatorPtr, position, rotation, scale, bounds] = registry.getMany<Animator, Position, Rotation, Scale, BoundingSphere>(entityId);
		Animator& animator = *animatorPtr;

		if (Math::isEqual(animator.playRate, 0.f) || animator.resourceId == ECS::INVALID_ID)
		{
			return;
		}

		if (animator.shouldCull && !CameraStatics::isInView(camera, *position, *rotation, *scale, *bounds))
		{
			return;
		}
		
		const Resource<Animation>* animationRes = registry.get<Resource<Animation>>(animator.resourceId);
		if (animationRes == nullptr || !animationRes->isReady)
		{
			return;
		}

		const Animation& animation = animationRes->value;
		if (animator.frameId == INVALID_FRAME_ID)
		{
			// play the first frame.
			animator.frameId = 0;
			updateEntity(registry, entityId, animation, animator.frameId);
			return;
		}

		animator.elapsed += time.delta;
		if (animator.elapsed < animator.playRate)
		{
			return;
		}

		const FrameId frameDelta = Math::floorToInt(time.delta / animator.playRate);
		animator.frameId += frameDelta;

		if (animator.frameId >= animation.frames.size())
		{
			switch (animator.mode)
			{
				case Animator::EPlayMode::Loop:
				{
					animator.frameId = 0;
					animator.elapsed = 0.f;
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
		
		updateEntity(registry, entityId, animation, animator.frameId);
	});
}
