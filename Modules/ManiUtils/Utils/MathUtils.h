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
	};
}