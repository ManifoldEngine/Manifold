#include "Animation.h"

using namespace Mani;

void Mani::Animations::play(ECS::Registry& registry, Mani::EntityId entityId, Mani::EntityId resourceId, Animator::EPlayMode playMode)
{
	Ref<Animator> animator = registry.find<Animator>(entityId);
	MANI_ASSERT(animator.isValid(), "Trying to play an animation on an entity that doesn't have an animation component.");

	animator->elapsed = 0.f;
	animator->frameId = INVALID_FRAME_ID;
	animator->mode = playMode;
	animator->resourceId = resourceId;
}
