#pragma once

#include <glm/glm.hpp>

namespace Mani
{
	class MathUtils
	{
	public:
		static glm::vec3 normalize(const glm::vec3& v)
		{
			if (glm::length2(v) < FLT_EPSILON)
			{
				return glm::vec3(0.f);
			}
			return glm::normalize(v);
		}

		static float mapRanges(float value, float minX, float maxX, float minY, float maxY)
		{
			MANI_ASSERT(std::abs(maxX - minX) > FLT_EPSILON, "Do not divide by zero.");
			const float slope = (maxY - maxX) / (maxX - minX);
			return minY + slope * (value - minX);
		}
	};
}