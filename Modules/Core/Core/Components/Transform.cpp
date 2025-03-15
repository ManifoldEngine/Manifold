#include "Transform.h"

using namespace Mani;

Mat4f Transform::calculateModelMatrix() const
{
	return MAT4F::IDENTITY.translate(position)
						  .rotate(rotation)
						  .scale(scale);
}

Vec3f Transform::forward() const
{
	return rotation.rotate(VEC3F::FORWARD).normalize();
}

Vec3f Transform::up() const
{
	return rotation.rotate(VEC3F::UP).normalize();
}

Vec3f Transform::right() const
{
	return rotation.rotate(VEC3F::RIGHT).normalize();
}
