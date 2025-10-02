#include "AnimationSystem.h"

#include <Core/TimeSystem.h>
#include <Core/Debug/Profiling.h>
#include <Core/Async/Parallel.h>

#include <Camera/Camera.h>
#include <Camera/CameraSystem.h>

#include <Animation/Animation.h>
#include <Animation/ShaderNames.h>

#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/Texture.h>
#include <RenderAPI/Shader.h>

#include <Resources/ResourceSystem.h>

using namespace Mani;

void updateEntity(ECS::Registry& registry, ECS::EntityId entityId, const Animation& animation, FrameId frameId)
{
	MANI_ASSERT(frameId <= animation.frames.size(), "frame id out of bounds");
	const Animation::Frame& frame = animation.frames[frameId];

	if (MeshComponent* meshComponent = registry.get<MeshComponent>(entityId))
	{
		meshComponent->textureParameters[ShaderNames::MANI_TEXTURE_0] = frame.textureId;
	}
}

void resetEntity(ECS::Registry& registry, ECS::EntityId entityId)
{
	if (MeshComponent* meshComponent = registry.get<MeshComponent>(entityId))
	{
		meshComponent->textureParameters.erase(ShaderNames::MANI_TEXTURE_0);
	}
}

void AnimationSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<TimeSystem>();
	world.initializeDependency<CameraSystem>();
	world.initializeDependency<ResourceSystem>();

	ResourceSystem::registerExtension(registry, &extension);
}

void AnimationSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	ResourceSystem::unregisterExtension(registry, &extension);
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

		const FrameId frameDelta = Math::floorToInt(animator.elapsed / animator.playRate);
		animator.frameId += frameDelta;
		const float frameDeltaInSeconds = animator.playRate * frameDelta;
		const float elapsedRemainder = animator.elapsed - frameDeltaInSeconds;

		if (animator.frameId >= animation.frames.size())
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
	const ECS::Entity* entity = registry.getEntity(entityId);
	if (entity == nullptr)
	{
		return;
	}

	const ECS::ComponentId animationId = registry.getComponentId<Resource<Animation>>();
	if (!entity->hasComponent(animationId))
	{
		return;
	}

	Resource<Animation>* animationRes = registry.get<Resource<Animation>>(entityId);
	MANI_ASSERT(animationRes != nullptr, "animation should be loaded at this point");
	Animation& animation = animationRes->value;
	for (auto& frame : animation.frames)
	{
		frame.textureId = ResourceSystem::loadResource<Texture>(registry, frame.texturePath, tag);
	}
	animationRes->isReady = true;
}

void AnimationResourceSystemExtension::onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	const ECS::Entity* entity = registry.getEntity(entityId);
	if (entity == nullptr)
	{
		return;
	}

	const ECS::ComponentId animationId = registry.getComponentId<Resource<Animation>>();
	if (!entity->hasComponent(animationId))
	{
		return;
	}

	Resource<Animation>* animationRes = registry.get<Resource<Animation>>(entityId);
	MANI_ASSERT(animationRes != nullptr && animationRes->isReady, "animation should be loaded at this point");
	Animation& animation = animationRes->value;
	for (auto& frame : animation.frames)
	{
		ResourceSystem::unloadResource(registry, frame.textureId);
	}
}
