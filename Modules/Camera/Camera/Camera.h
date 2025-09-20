#pragma once

#include <Core/ManiAssert.h>
#include <Core/Vec.h>

#include <Core/Components/Position.h>
#include <Core/Components/Rotation.h>
#include <Core/Components/Scale.h>

#include <RenderAPI/BoundingSphere.h>

#include <ManiMaths/Fwd.h>
#include <Log.h>
#include <string_view>

namespace Mani
{
	const std::string_view LogCamera = "LogCamera";

	enum class ECameraMode : uint8_t
	{
		PERSPECTIVE = 0,
		ORTHOGRAPHIC
	};

	struct Frustum
	{
		struct Plane
		{
			Vec3f point = VEC3F::ZERO;
			Vec3f normal = VEC3F::ZERO;
		};

		Plane top;
		Plane bottom;
		Plane right;
		Plane left;
		Plane far;
		Plane near;
	};

	struct Camera
	{
		float fov = 45.f;
		float near = .1f;
		float far = 10'000.f;
		float width = 1280.f;
		float height = 720.f;
		
		ECameraMode mode = ECameraMode::PERSPECTIVE;
		Mat4f projection = MAT4F::IDENTITY;
		Mat4f view = MAT4F::IDENTITY;

		Frustum frustrum;

		// Ortho specific parameters
		uint32_t pixelsPerUnit = 32;

		// virtual resolution overwrite the pixels per units value
		bool useVirtualResolution = false;
		float virtualWidth = 480.f;
		float virtualHeight = 640.f;
	};

	namespace CameraStatics
	{
		float getAspectRatio(const Camera& camera);
		Vec2f worldToScreenSpace(const Camera& camera, const Vec3f& position);
		Vec3f cameraPixelsToWorldSpace(const Camera& camera, const Vec2f& position, bool shouldClampPosition = true);
		Vec3f screenToWorldSpace(const Camera& camera, Vec2f position);

		bool isInView(const Camera& camera, const Position& position, const Rotation& rotation, const Scale& scale, const BoundingSphere& boundingSphere);
	}

	namespace FrustumStatics
	{
		Frustum create(const Camera& camera, const Vec3f& position, const Quatf& rotation);

		bool isSphereInside(const Frustum& frustum, const Vec3f& position, const Vec3f& scale, float radius);
		bool isSphereOnOrForwardPlane(const Vec3f& position, float radius, const Frustum::Plane& plane);
		float getSignedDistanceToPlane(const Vec3f& point, const Frustum::Plane& plane);
	}
}