#pragma once

#include <glm/fwd.hpp>
#include <vector>

namespace Mani
{
	struct MeshColliderComponent
	{
		std::vector<glm::vec3> vertices;
	};
}