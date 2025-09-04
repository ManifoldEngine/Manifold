#pragma once

#include <string_view>
#include <Core/ManiAssert.h>
#include <ManiMaths/Fwd.h>
#include <Log.h>

namespace Mani
{
	const std::string_view LogCamera = "LogCamera";

	struct Camera
	{
		enum class EMode : uint8_t
		{
			PERSPECTIVE = 0,
			ORTHOGRAPHIC
		};

		float fov = 45.f;
		float nearClipPlane = .1f;
		float farClipPlane = 10'000.f;
		float width = 800.f;
		float height = 800.f;
		EMode mode = EMode::PERSPECTIVE;

		float getAspectRatio() const
		{
			MANI_ASSERT(Math::abs(height) > FLT_EPSILON, "height cannot be zero");
			return width / height;
		}

		Mat4f projection = MAT4F::IDENTITY;
		Mat4f view = MAT4F::IDENTITY;

		Vec2f worldToScreenSpace(const Vec3f& position) const
		{
			Vec4f projectedPosition = position.homogenous() * view;

			if (Math::isEqual(projectedPosition.w, 0.f) ||
				projectedPosition.w == 0.f) // this fixes warning C4723: potential divide by 0
			{
				return VEC2F::ZERO;
			}
			return Vec2f(projectedPosition.x / projectedPosition.w, projectedPosition.y / projectedPosition.w);
		}

		Vec3f screenToWorldSpace(const Vec2f& position) const
		{
			Vec4f projectedPosition = (projection * view).inverse() * position.homogenous();
			if (Math::abs(projectedPosition.w) <= FLT_EPSILON)
			{
				return VEC3F::ZERO;
			}
			
			return Vec3f {
				projectedPosition.x / projectedPosition.w,
				0.f,
				projectedPosition.z / projectedPosition.w,
			};
		}
	};
}