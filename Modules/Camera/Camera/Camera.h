#pragma once

#include <string_view>
#include <Core/CoreAssert.h>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	const std::string_view LogCamera = "LogCamera";

	struct Camera
	{
		float fov = 45.f;
		float nearClipPlane = .1f;
		float farClipPlane = 10'000.f;
		float width = 800.f;
		float height = 800.f;

		float getAspectRatio() const
		{
			MANI_ASSERT(Math::abs(height) > FLT_EPSILON, "height cannot be zero");
			return width / height;
		}

		Mat4f projection;
		Mat4f view;

		Vec2f worldToScreenSpace(const Vec3f& position) const
		{
			Vec4f projectedPosition = position.homogenous() * view;

			if (Math::abs(projectedPosition.w) <= FLT_EPSILON)
			{
				return VEC2F::ZERO;
			}

			return Vec2f(projectedPosition.x / projectedPosition.w, projectedPosition.y / projectedPosition.w);
		}
	};
}