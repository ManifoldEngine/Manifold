#include <GJK.h>

using namespace Mani;

bool GJK::solve(const std::vector<glm::vec3>& shape1, const Transform* transform1, const std::vector<glm::vec3>& shape2, const Transform* transform2, Simplex& outSimplex)
{
	constexpr size_t MAX_ITERATION = 32;
	MANI_ASSERT(shape1.size() > 1 && shape2.size() > 1, "Shapes cannot be empty");
	MANI_ASSERT(transform1 != nullptr && transform2 != nullptr, "Transforms cannot be nullptr");

	const glm::mat4 matrix1 = transform1->calculateModelMatrix();
	const glm::mat4 matrix2 = transform2->calculateModelMatrix();
	const glm::quat& rotation1 = transform1->rotation;
	const glm::quat& rotation2 = transform2->rotation;

	auto getSupport = [&shape1, &rotation1, &matrix1, &shape2, &rotation2, &matrix2] (const glm::vec3& direction) -> glm::vec3
	{
		return GJK::getSupport(shape1, rotation1, matrix1, shape2, rotation2, matrix2, direction);
	};

	glm::vec3 support = getSupport(glm::vec3(1.f, 0.f, 0.f));

	outSimplex.a = support;
	outSimplex.count = 1;

	glm::vec3 direction = -support;

	for (size_t iteration = 0; iteration < MAX_ITERATION; iteration++)
	{
		support = getSupport(direction);
		if (glm::dot(support, direction) < 0)
		{
			// support is opposed from direction, the origin cannot be in the outSimplex.
			return false;
		}

		// push simplex points
		outSimplex.d = outSimplex.c;
		outSimplex.c = outSimplex.b;
		outSimplex.b = outSimplex.a;
		outSimplex.count++;

		outSimplex.a = support;

		if (resolveNextDirection(outSimplex, direction))
		{
			// a collision have been found.
			return true;
		}
	}

	MANI_ASSERT(false, "We've reached max iteration, that should not happen.");
}

glm::vec3 GJK::getSupport(const std::vector<glm::vec3>& shape1, const glm::quat& rotation1, const glm::mat4& transform1, const std::vector<glm::vec3>& shape2, const glm::quat& rotation2, const glm::mat4& transform2, const glm::vec3& direction)
{
	return GJK::getFurthestPoint(shape1, rotation1, transform1, direction) - GJK::getFurthestPoint(shape2, rotation2, transform2, -direction);
}

glm::vec3 GJK::getFurthestPoint(const std::vector<glm::vec3>& shape, const glm::quat& rotation, const glm::mat4& transformMatrix, const glm::vec3& direction)
{
	// transform direction to local direction and find the support point.
	const glm::vec3 localDirection = glm::inverse(rotation) * glm::vec4(direction.x, direction.y, direction.z, 1.f);

	size_t maxVertexIndex = 0;
	float maxDistance = -FLT_MAX;
	for (size_t index = 0; index < shape.size(); index++)
	{
		const float distance = glm::dot(shape[index], localDirection);
		if (distance > maxDistance)
		{
			maxDistance = distance;
			maxVertexIndex = index;
		}
	}

	// transform the support point back into world space
	const glm::vec3 vertex = shape[maxVertexIndex];
	return transformMatrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.f);
}

bool GJK::resolveNextDirection(Simplex& simplex, glm::vec3& outDirection)
{
	switch (simplex.count)
	{
		case 2: return solveSimplex2(simplex, outDirection);
		case 3: return solveSimplex3(simplex, outDirection);
		case 4: return solveSimplex4(simplex, outDirection);

		default:
		{
			MANI_ASSERT(false, "Invalid simplex dimensions");
			break;
		}
	}
	return false;
}

bool GJK::solveSimplex2(Simplex& simplex, glm::vec3& outDirection)
{
	MANI_ASSERT(simplex.count == 2, "solving for ab");

	const glm::vec3 ao = -simplex.a;
	const glm::vec3 ab = simplex.b - simplex.a;

	if (isSameDirection(ab, ao))
	{
		// origin is on the side of ab, we continue.
		outDirection = glm::cross(glm::cross(ab, ao), ab);
	}
	else
	{
		// a is closest to origin
		simplex.count = 1;
		outDirection = ao;
	}

	return false;
}

bool GJK::solveSimplex3(Simplex& simplex, glm::vec3& outDirection)
{
	MANI_ASSERT(simplex.count == 3, "solving for abc");

	const glm::vec3 ao = -simplex.a;
	const glm::vec3 ab = simplex.b - simplex.a;
	const glm::vec3 ac = simplex.c - simplex.a;

	const glm::vec3 abc = glm::cross(ab, ac);

	if (isSameDirection(glm::cross(abc, ac), ao))
	{
		if (isSameDirection(ac, ao))
		{
			// ac is closest to origin
			simplex.b = simplex.c;
			simplex.count = 2;
			outDirection = glm::cross(glm::cross(ac, ao), ac);
		}
		else
		{
			// ab is closest to origin
			simplex.count = 2;
			return solveSimplex2(simplex, outDirection);
		}
	}
	else
	{
		if (isSameDirection(glm::cross(ab, abc), ao))
		{
			// ab is closest to origin
			simplex.count = 2;
			return solveSimplex2(simplex, outDirection);
		}
		else
		{
			// origin is inside abc
			if (isSameDirection(abc, ao))
			{
				outDirection = abc;
			}
			else
			{
				// acb is closest to origin
				// swap b and c
				simplex.d = simplex.b;
				simplex.b = simplex.c;
				simplex.c = simplex.d;

				outDirection = -abc;
			}
		}
	}

	return false;
}

bool GJK::solveSimplex4(Simplex& simplex, glm::vec3& outDirection)
{
	MANI_ASSERT(simplex.count == 4, "solving for abcd");

	const glm::vec3 ao = -simplex.a;
	const glm::vec3 ab = simplex.b - simplex.a;
	const glm::vec3 ac = simplex.c - simplex.a;

	const glm::vec3 abc = glm::cross(ab, ac);
	
	if (isSameDirection(abc, ao))
	{
		// triangle abc is facing origin
		simplex.count = 3;
		
		// find a new 4th support point.
		return solveSimplex3(simplex, outDirection);
	}

	const glm::vec3 ad = simplex.c - simplex.a;
	const glm::vec3 acd = glm::cross(ac, ad);

	if (isSameDirection(acd, ao))
	{
		// triangle acd is facing origin
		simplex.b = simplex.c;
		simplex.c = simplex.d;
		simplex.count = 3;

		// find a new 4th support point.
		return solveSimplex3(simplex, outDirection);
	}

	const glm::vec3 adb = glm::cross(ad, ab);

	if (isSameDirection(adb, ao))
	{
		// triangle abd is facing origin
		simplex.b = simplex.d;
		simplex.c = simplex.b;
		simplex.count = 3;

		// find a new 4th support point.
		return solveSimplex3(simplex, outDirection);
	}

	return true;
}

bool GJK::isSameDirection(const glm::vec3& direction1, const glm::vec3& direction2)
{
	return glm::dot(direction1, direction2) > 0;
}
