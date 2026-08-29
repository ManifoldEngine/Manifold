#include "Camera.h"

#include <Core/ECS/View.h>
#include <Core/Transform.h>
#include <Core/Profiling/Profiling.h>

using namespace Mani;

ECS::EntityId Cameras::getMainCameraId(ECS::Registry& registry)
{
	ECS::ConstView<MainCamera, Camera> view(registry);
	return view.begin().getEntityId();
}

ECS::EntityId Cameras::createMainCamera(ECS::Registry& registry)
{
	// create Camera
	ECS::EntityId cameraId = registry.create();

	registry.add<Mani::Camera>(cameraId);
	registry.add<Mani::Position>(cameraId, Mani::VEC3F::BACK * 5.f); // film the origin by default
	registry.add<Mani::Rotation>(cameraId);
	registry.add<Mani::MainCamera>(cameraId);

	return cameraId;
}

float Cameras::getAspectRatio(const Camera& camera)
{
	MANI_ASSERT(!Math::isEqual(camera.height, 0.f), "height cannot be zero");
    return camera.width / camera.height;
}

Vec2f Cameras::worldToScreenSpace(const Camera& camera, const Vec3f& position)
{
	Vec4f projectedPosition = camera.view * camera.projection * position.homogenous();

	if (Math::isEqual(projectedPosition.w, 0.f) ||
		projectedPosition.w == 0.f) // this fixes warning C4723: potential divide by 0
	{
		return VEC2F::ZERO;
	}

	return Vec2f(projectedPosition.x / projectedPosition.w, projectedPosition.y / projectedPosition.w);
}

Vec3f Cameras::cameraPixelsToWorldSpace(const Camera& camera, const Vec2f& position, bool shouldClampPosition)
{
	MANI_ASSERT(!Math::isEqual(camera.width, 0) && !Math::isEqual(camera.height, 0), "Do not divide by zero");
	const float halfWidth = camera.width / 2.f;
	const float halfHeight = camera.height / 2.f;

	float clampedX = position.x;
	float clampedY = position.y;
	
	if (shouldClampPosition)
	{
		// clamp to camera size
		clampedX = Math::clamp(position.x, 0.f, camera.width);
		clampedY = Math::clamp(position.y, 0.f, camera.height);
	}

	// transform to screen space
	const float x = (clampedX - halfWidth) / halfWidth;
	const float y = (clampedY - halfHeight) / halfHeight;

	return Cameras::screenToWorldSpace(camera, Vec2f{ x, y });
}

Vec3f Cameras::screenToWorldSpace(const Camera& camera, Vec2f position)
{
	// we reverse y because the screen coordinates go from top to bottom
	position.y *= -1.f;
	Vec4f projectedPosition = (camera.projection * camera.view).inverse() * position.homogenous();
	if (Math::abs(projectedPosition.w) <= FLT_EPSILON)
	{
		return VEC3F::ZERO;
	}

	return Vec3f
	{
		projectedPosition.x / projectedPosition.w,
		projectedPosition.y / projectedPosition.w,
		projectedPosition.z / projectedPosition.w,
	};
}

bool Cameras::isInView(const Camera& camera, const Position& position, const Rotation& rotation, const Scale& scale, const BoundingSphere& boundingSphere)
{
	MANI_TIME_SCOPE("FrustumStatics_isInView");
	return FrustumStatics::isSphereInside(camera.frustrum, position, scale, boundingSphere.radius);
}

Frustum FrustumStatics::create(const Camera& camera, const Vec3f& position, const Quatf& rotation)
{
	MANI_TIME_SCOPE("FrustumStatics_create");

	Frustum frustum;
	
	const Vec3f forward = Transform::forward(rotation);
	const Vec3f up = Transform::up(rotation);
	const Vec3f right = Transform::right(rotation);

	switch (camera.mode)
	{
		case ECameraMode::Perspective:
		{
			const float aspectRatio = Cameras::getAspectRatio(camera);
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

		case ECameraMode::Orthographic:
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
