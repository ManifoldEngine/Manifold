#include "Transform.h"
#include <Core/Vec.h>
#include <Core/ManiAssert.h>

using namespace Mani;

Mat4f Transform::model(const Position& p, const Rotation& q, const Scale& s)
{
	return MAT4F::IDENTITY.translate(p.value)
						  .rotate(q.value)
						  .scale(s.value);
}

Vec3f Transform::forward(const Rotation& q)
{
	return q.value.rotate(VEC3F::FORWARD).normalize();
}

Vec3f Transform::up(const Rotation& q)
{
	return q.value.rotate(VEC3F::UP).normalize();
}

Vec3f Transform::right(const Rotation& q)
{
	return q.value.rotate(VEC3F::RIGHT).normalize();
}

std::tuple<Position&, Rotation&, Scale&> Mani::Transform::addTransform(ECS::Registry& registry, ECS::EntityId entityId)
{
	MANI_ASSERT(registry.isValid(entityId), "Trying to add a transform to an invalid entity.");
	Position& position = *registry.add<Position>(entityId);
	Rotation& rotation = *registry.add<Rotation>(entityId);
	Scale& scale = *registry.add<Scale>(entityId);
	return { position, rotation, scale };
}

std::tuple<Position*, Rotation*, Scale*> Mani::Transform::getTransform(ECS::Registry& registry, ECS::EntityId entityId)
{
	Position* position = registry.get<Position>(entityId);
	Rotation* rotation = registry.get<Rotation>(entityId);
	Scale* scale = registry.get<Scale>(entityId);
	return { position, rotation, scale };
}
