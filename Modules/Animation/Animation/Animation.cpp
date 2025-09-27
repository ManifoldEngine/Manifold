#include "Animation.h"

using namespace Mani;

void Mani::AnimationStatics::play(ECS::Registry& registry, Mani::ECS::EntityId entityId, Mani::ECS::EntityId resourceId, Animator::EPlayMode playMode)
{
	Animator* animator = registry.get<Animator>(entityId);
	MANI_ASSERT(animator != nullptr, "Trying to play an animation on an entity that doesn't have an animation component.");

	animator->elapsed = 0.f;
	animator->frameId = INVALID_FRAME_ID;
	animator->mode = playMode;
	animator->resourceId = resourceId;
}
