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

Mat4f Transform::model(const Vec3f& p, const Quatf& q, const Vec3f& s)
{
	return MAT4F::IDENTITY.translate(p)
		.rotate(q)
		.scale(s);
}

Vec3f Transform::forward(const Quatf& q)
{
	return q.rotate(VEC3F::FORWARD).normalize();
}

Vec3f Transform::up(const Quatf& q)
{
	return q.rotate(VEC3F::UP).normalize();
}

Vec3f Transform::right(const Quatf& q)
{
	return q.rotate(VEC3F::RIGHT).normalize();
}
