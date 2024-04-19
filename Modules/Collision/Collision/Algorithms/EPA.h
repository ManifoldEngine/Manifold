#pragma once

#include <vector>
#include <glm/fwd.hpp>
#include <Core/CoreFwd.h>
#include <Collision/HitResult.h>
#include <Collision/Algorithms/Simplex.h>

namespace Mani
{
	class EPA
	{
	public:
		static bool solve(
			const std::vector<glm::vec3>& shape1, const Transform* transform1,
			const std::vector<glm::vec3>& shape2, const Transform* transform2,
			const Simplex& simplex,
			HitResult& outHitResult
		);

	private:
		static void getFaceNormals(const std::vector<glm::vec3>& polytope, const std::vector<size_t>& faces, std::vector<glm::vec3>& outNormals, std::vector<float>& outDistances, size_t& outMinTriangleIndex);
		static void addEdgeIfUniqueElseRemove(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t edgeIndex1, size_t edgeIndex2);
	};
}