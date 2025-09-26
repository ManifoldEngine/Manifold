#include "Animation.h"

using namespace Mani;

void AnimationStatics::play(Mani::ECS::Registry& registry, Mani::ECS::EntityId entityId)
{
	Animation* animation = registry.get<Animation>(entityId);
	MANI_ASSERT(animation != nullptr, "Trying to play an animation on an entity that doesn't have an animation component.");
}
