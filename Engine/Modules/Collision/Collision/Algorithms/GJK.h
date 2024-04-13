#pragma once

#include <vector>
#include <glm/fwd.hpp>
#include <Core/CoreFwd.h>
#include <Collision/Algorithms/Simplex.h>

namespace Mani
{
	class GJK
	{
	public:
		static bool solve(
			const std::vector<glm::vec3>& shape1, const Transform* transform1, 
			const std::vector<glm::vec3>& shape2, const Transform* transform2, 
			Simplex& outSimplex
		);

		static glm::vec3 getSupport(const std::vector<glm::vec3>& shape1, const glm::quat& rotation1, const glm::mat4& transform1, const std::vector<glm::vec3>& shape2, const glm::quat& rotation2, const glm::mat4& transform2, const glm::vec3& direction);

		static bool isSameDirection(const glm::vec3& direction1, const glm::vec3& direction2);
	private:
		static glm::vec3 getFurthestPoint(const std::vector<glm::vec3>& shape, const glm::quat& rotation, const glm::mat4& transformMatrix, const glm::vec3& direction);
		
		static bool resolveNextDirection(Simplex& simplex, glm::vec3& outDirection);
		
		static bool solveSimplex2(Simplex& simplex, glm::vec3& outDirection);
		static bool solveSimplex3(Simplex& simplex, glm::vec3& outDirection);
		static bool solveSimplex4(Simplex& simplex, glm::vec3& outDirection);	
	};
}