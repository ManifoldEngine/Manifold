#include "Camera.h"

#include <Core/Transform.h>
#include <Core/Debug/Profiling.h>

using namespace Mani;

float CameraStatics::getAspectRatio(const Camera& camera)
{
	MANI_ASSERT(!Math::isEqual(camera.height, 0.f), "height cannot be zero");
    return camera.width / camera.height;
}

Vec2f CameraStatics::worldToScreenSpace(const Camera& camera, const Vec3f& position)
{
	Vec4f projectedPosition = position.homogenous() * camera.view;

	if (Math::isEqual(projectedPosition.w, 0.f) ||
		projectedPosition.w == 0.f) // this fixes warning C4723: potential divide by 0
	{
		return VEC2F::ZERO;
	}
	return Vec2f(projectedPosition.x / projectedPosition.w, projectedPosition.y / projectedPosition.w);
}

Vec3f CameraStatics::screenToWorldSpace(const Camera& camera, const Vec2f& position)
{
	Vec4f projectedPosition = (camera.projection * camera.view).inverse() * position.homogenous();
	if (Math::abs(projectedPosition.w) <= FLT_EPSILON)
	{
		return VEC3F::ZERO;
	}

	return Vec3f{
		projectedPosition.x / projectedPosition.w,
		0.f,
		projectedPosition.z / projectedPosition.w,
	};
}

bool Mani::CameraStatics::isInView(const Camera& camera, const Position& position, const Rotation& rotation, const Scale& scale, const BoundingSphere& boundingSphere)
{
	MANI_TIME_SCOPE(FrustumStaticsisInView);
	return FrustumStatics::isSphereInside(camera.frustrum, position.value, scale.value, boundingSphere.radius);
}

Frustum FrustumStatics::create(const Camera& camera, const Vec3f& position, const Quatf& rotation)
{
	MANI_TIME_SCOPE(FrustumStaticscreate);

	Frustum frustum;
	
	const Vec3f forward = Transform::forward(rotation);
	const Vec3f up = Transform::up(rotation);
	const Vec3f right = Transform::right(rotation);

	switch (camera.mode)
	{
		case ECameraMode::PERSPECTIVE:
		{
			const float aspectRatio = CameraStatics::getAspectRatio(camera);
			const float halfVerticalSize = camera.far * Math::tan(camera.fov);
			const float halfHorizontalSize = halfVerticalSize * aspectRatio;
			const Vec3f farForward = camera.far * forward;

			frustum.near =		{ .point = position + camera.near * forward,	.normal = forward													};
			frustum.far =		{ .point = position + farForward,				.normal = forward * -1.f											};
			frustum.right =		{ .point = position,							.normal = Vec3f::cross(farForward - right * halfHorizontalSize, up) };
			frustum.left =		{ .point = position,							.normal = Vec3f::cross(up, farForward + right * halfHorizontalSize) };
			frustum.top =		{ .point = position, 							.normal = Vec3f::cross(right, farForward - up * halfVerticalSize)	};
			frustum.bottom =	{ .point = position,							.normal = Vec3f::cross(farForward + up * halfVerticalSize, right)	};
			return frustum;
		}

		case ECameraMode::ORTHOGRAPHIC:
		{
			MANI_ASSERT(!Math::isEqual(camera.pixelsPerUnit, 0.f), "Do not divide by zero");
			const float width			= camera.width / camera.pixelsPerUnit;
			const float height			= camera.height / camera.pixelsPerUnit;

			const float halfWidth		= width / 2.f;
			const float halfHeight		= height / 2.f;

			const Vec3f nearCenter		= position + forward * camera.near;
			const Vec3f farCenter		= position + forward * camera.far;

			const Vec3f nearTopLeft		= nearCenter + up * halfHeight - right * halfWidth;
			const Vec3f nearTopRight	= nearCenter + up * halfHeight + right * halfWidth;
			const Vec3f nearBottomLeft	= nearCenter - up * halfHeight - right * halfWidth;
			const Vec3f nearBottomRight = nearCenter - up * halfHeight + right * halfWidth;

			const Vec3f farTopLeft		= farCenter + up * halfHeight - right * halfWidth;
			const Vec3f farTopRight		= farCenter + up * halfHeight + right * halfWidth;
			const Vec3f farBottomLeft	= farCenter - up * halfHeight - right * halfWidth;
			const Vec3f farBottomRight	= farCenter - up * halfHeight + right * halfWidth;

			frustum.near =		{ .point = nearCenter,		.normal = forward.normalize()																			};
			frustum.far =		{ .point = farCenter,		.normal = forward.normalize() * -1.f																	};
			frustum.right =		{ .point = nearTopRight,	.normal = Vec3f::cross(farBottomRight - nearTopRight, nearBottomRight - nearTopRight).normalize()		};
			frustum.left =		{ .point = nearTopLeft,		.normal = Vec3f::cross(nearBottomLeft - nearTopLeft, farBottomLeft - nearTopLeft).normalize()			};
			frustum.top =		{ .point = nearTopLeft,		.normal = Vec3f::cross(farTopRight - nearTopLeft, nearTopRight - nearTopLeft).normalize()				};
			frustum.bottom =	{ .point = nearBottomLeft,	.normal = Vec3f::cross(nearBottomRight - nearBottomLeft, farBottomRight - nearBottomLeft).normalize()	};
			
			return frustum;
		}

		default:
		{
			MANI_ASSERT(false, "Unsupported camera mode");
			return frustum;
		}
	}
}

bool FrustumStatics::isSphereInside(const Frustum& frustum, const Vec3f& position, const Vec3f& scale, float radius)
{
	const float maxScale = Math::maxT(Math::maxT(scale.x, scale.y), scale.z);
	const float scaledRadius = radius * maxScale;
	return isSphereOnOrForwardPlane(position, scaledRadius, frustum.near) &&
		isSphereOnOrForwardPlane(position, scaledRadius, frustum.far) &&
		isSphereOnOrForwardPlane(position, scaledRadius, frustum.right) &&
		isSphereOnOrForwardPlane(position, scaledRadius, frustum.left) &&
		isSphereOnOrForwardPlane(position, scaledRadius, frustum.top) &&
		isSphereOnOrForwardPlane(position, scaledRadius, frustum.bottom);
}

bool FrustumStatics::isSphereOnOrForwardPlane(const Vec3f& position, float radius, const Frustum::Plane& plane)
{
	return getSignedDistanceToPlane(position, plane) > -radius;
}

float FrustumStatics::getSignedDistanceToPlane(const Vec3f& point, const Frustum::Plane& plane)
{
	return Vec3f::dot(plane.normal, point - plane.point);
}
