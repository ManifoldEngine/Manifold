#pragma once

#include <glm/fwd.hpp>

namespace Mani
{
	struct HitResult
	{
		bool isHit = false;
		glm::vec3 normal = glm::vec3(0.f);
		float penetrationDepth = 0.f;
	};
}