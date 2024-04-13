#include <EPA.h>

#include <Collision/Algorithms/GJK.h>
#include <array>

using namespace Mani;

bool EPA::solve(const std::vector<glm::vec3>& shape1, const Transform* transform1, const std::vector<glm::vec3>& shape2, const Transform* transform2, const Simplex& simplex, HitResult& outHitResult)
{
	constexpr size_t MAX_ITERATION = 32;

	const glm::mat4 matrix1 = transform1->calculateModelMatrix();
	const glm::mat4 matrix2 = transform2->calculateModelMatrix();
	const glm::quat& rotation1 = transform1->rotation;
	const glm::quat& rotation2 = transform2->rotation;

	std::vector<glm::vec3> polytope;
	polytope.push_back(simplex.a);
	polytope.push_back(simplex.b);
	polytope.push_back(simplex.c);
	polytope.push_back(simplex.d);

	std::vector<size_t> faces =
	{
		0, 1, 3,
		0, 3, 1,
		0, 2, 3,
		1, 3, 2
	};

	constexpr float TOLERANCE = 0.0001f;

	std::vector<glm::vec3> normals;
	std::vector<float> distances;
	size_t minFaceIndex = 0;
	getFaceNormals(polytope, faces, normals, distances, minFaceIndex);

	glm::vec3 minNormal = glm::vec3(0.f);
	float minDistance = FLT_MAX;
	for (size_t iteration = 0; iteration < MAX_ITERATION; iteration++)
	{
		minNormal = normals[minFaceIndex];
		minDistance = distances[minFaceIndex];

		if (minDistance < FLT_MAX)
		{
			outHitResult.normal = glm::normalize(minNormal);
			outHitResult.penetrationDepth = minDistance + TOLERANCE;
			outHitResult.isHit = true;
			return true;
		}

		const glm::vec3 support = GJK::getSupport(shape1, rotation1, matrix1, shape2, rotation2, matrix2, minNormal);
		const float supportDistance = glm::dot(minNormal, support);
		if (abs(supportDistance - minDistance) <= TOLERANCE)
		{
			continue;
		}

		minDistance = FLT_MAX;
			
		std::vector<std::pair<size_t, size_t>> uniqueEdges;
		for (size_t i = 0; i < normals.size(); i++)
		{
			size_t f = i * 3; // face index

			//if (!GJK::isSameDirection(normals[i], support))
			if (glm::dot(normals[i], support) <= glm::dot(normals[i], polytope[faces[f]]))
			{
				continue;
			}

			addEdgeIfUniqueElseRemove(uniqueEdges, faces, f		, f + 1	);
			addEdgeIfUniqueElseRemove(uniqueEdges, faces, f	+ 1	, f + 2	);
			addEdgeIfUniqueElseRemove(uniqueEdges, faces, f	+ 2	, f		);

			faces[f + 2] = faces.back(); faces.pop_back();
			faces[f + 1] = faces.back(); faces.pop_back();
			faces[f	   ] = faces.back(); faces.pop_back();

			normals[i] = normals.back(); normals.pop_back();
			i--;
		}

		std::vector<size_t> newFaces;
		for (const auto& [edgeIndexA, edgeIndexB] : uniqueEdges)
		{
			newFaces.push_back(edgeIndexA);
			newFaces.push_back(edgeIndexB);
			newFaces.push_back(polytope.size());
		}

		polytope.push_back(support);

		std::vector<glm::vec3> newNormals;
		std::vector<float> newDistances;
		size_t newMinFaceIndex = 0;
		getFaceNormals(polytope, newFaces, newNormals, newDistances, newMinFaceIndex);

		float oldMinDistance = FLT_MAX;
		for (size_t i = 0; i < normals.size(); i++)
		{
			if (distances[i] < oldMinDistance)
			{
				oldMinDistance = distances[i];
				minFaceIndex = i;
			}
		}

		if (newDistances[newMinFaceIndex] < oldMinDistance)
		{
			minFaceIndex = newMinFaceIndex + normals.size();
		}

		faces.insert(faces.end(), newFaces.begin(), newFaces.end());
		normals.insert(normals.end(), newNormals.begin(), newNormals.end());
		distances.insert(distances.end(), newDistances.begin(), newDistances.end());
	}

	MANI_ASSERT(false, "We've reached max iteration, that should not happen.");
}

void EPA::getFaceNormals(const std::vector<glm::vec3>& polytope, const std::vector<size_t>& faces, std::vector<glm::vec3>& outNormals, std::vector<float>& outDistances, size_t& outMinTriangleIndex)
{
	float minDistance = FLT_MAX;

	for (size_t i = 0; i < faces.size(); i += 3)
	{
		const glm::vec3& a = polytope[faces[i]];
		const glm::vec3& b = polytope[faces[i + 1]];
		const glm::vec3& c = polytope[faces[i + 2]];

		const glm::vec3 abc = glm::cross(b - a, c - a);
		MANI_ASSERT(!glm::isnan(abc).b, "Simplex should have unique vertices");
		glm::vec3 normal = glm::normalize(abc);
		float distance = glm::dot(normal, a);

		if (distance < 0)
		{
			// make sure the normal is correctly oriented
			normal *= -1.f;
			distance *= -1.f;
		}

		outNormals.emplace_back(normal);
		outDistances.emplace_back(distance);

		if (distance < minDistance)
		{
			outMinTriangleIndex = i / 3;
			minDistance = distance;
		}
	}
}

void EPA::addEdgeIfUniqueElseRemove(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b)
{
	// if edge ab already exists from an adjacent face, then we should find ba in the edges vector
	//        <--
	//      a--<--d
	//     / \   /
	//    /   \ / 
	//   c-->--b
	//     -->
	auto baIterator = std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));

	if (baIterator == edges.end())
	{
		// edge ba doesn't exist, add ab
		edges.emplace_back(faces[a], faces[b]);
	}
	else
	{
		// edge ab already exists, remove it.
		edges.erase(baIterator);
	}
}
