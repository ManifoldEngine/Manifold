#pragma once

#include <Core/ManiAssert.h>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	inline Vec3f clampLenght(const Vec3f& v, float min, float max)
	{
		const float length = v.length();
		if (length < min)
		{
			return v.normalize() * min;
		}
		if (length > max)
		{
			return v.normalize() * max;
		}
		return v;
	}

	inline float mapRanges(float value, float minX, float maxX, float minY, float maxY)
	{
		MANI_ASSERT(Math::abs(maxX - minX) > FLT_EPSILON, "Do not divide by zero.");
		const float slope = (maxY - maxX) / (maxX - minX);
		return minY + slope * (value - minX);
	}
}