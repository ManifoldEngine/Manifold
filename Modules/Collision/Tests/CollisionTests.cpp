#include <ManiTests/ManiTests.h>
#include <Core/CoreFwd.h>
#include <Collision/Algorithms/GJK.h>
#include <Collision/Algorithms/EPA.h>

extern "C" __declspec(dllexport) void runTests()
{
	ManiTests::ManiTestsRunner::runTests();
}

using namespace Mani;

void addIfUnique(std::vector<glm::vec3>& vectors, const glm::vec3& v)
{
	auto equals = [](const glm::vec3& v1, const glm::vec3 v2) -> bool
	{
		return abs(v1.x - v2.x) <= FLT_EPSILON &&
			abs(v1.y - v2.y) <= FLT_EPSILON &&
			abs(v1.z - v2.z) <= FLT_EPSILON;
	};

	for (const auto& vector : vectors)
	{
		if (equals(vector, v))
		{
			return;
		}
	}

	vectors.push_back(v);
}
/*
MANI_SECTION_BEGIN(Collision, "Collision")
{
	MANI_SECTION_BEGIN(GJKTest, "GJK")
	{
		MANI_TEST(SimpleNoCollision, "simple case with no collision")
		{
			std::vector<glm::vec3> shape1 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};

			Transform transform1;
			transform1.position = glm::vec3(1.f);

			std::vector<glm::vec3> shape2 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};
			Transform transform2;
			transform2.position = glm::vec3(50.f);

			Simplex simplex;
			const bool result = GJK::solve(shape1, &transform1, shape2, &transform2, simplex);
			MANI_TEST_ASSERT(result == false, "Shape aren't colliding");

			std::vector<glm::vec3> set;
			addIfUnique(set, simplex.a);
			addIfUnique(set, simplex.b);
			addIfUnique(set, simplex.c);
			addIfUnique(set, simplex.d);
			MANI_TEST_ASSERT(set.size() == 4, "all simplex points must be unique");
		}

		MANI_TEST(SimpleYesCollision, "simple case with collision")
		{
			std::vector<glm::vec3> shape1 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};
			Transform transform1;
			transform1.position = glm::vec3(1.f);

			std::vector<glm::vec3> shape2 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};

			Transform transform2;
			transform2.position = glm::vec3(1.5f);
			
			Simplex simplex;
			const bool result = GJK::solve(shape1, &transform1, shape2, &transform2, simplex);
			MANI_TEST_ASSERT(result == true, "Shape are colliding");

			std::vector<glm::vec3> set;
			addIfUnique(set, simplex.a);
			addIfUnique(set, simplex.b);
			addIfUnique(set, simplex.c);
			addIfUnique(set, simplex.d);
			MANI_TEST_ASSERT(set.size() == 4, "all simplex points must be unique");
		}

		MANI_TEST(SimpleYesCollisionAtOrigin, "simple case with collision, but at world origin")
		{
			std::vector<glm::vec3> shape1 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};
			Transform transform1;
			transform1.position = glm::vec3(0.f);

			std::vector<glm::vec3> shape2 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};

			Transform transform2;
			transform2.position = glm::vec3(0.f);

			Simplex simplex;
			const bool result = GJK::solve(shape1, &transform1, shape2, &transform2, simplex);
			MANI_TEST_ASSERT(result == true, "Shape are colliding");

			std::vector<glm::vec3> set;
			addIfUnique(set, simplex.a);
			addIfUnique(set, simplex.b);
			addIfUnique(set, simplex.c);
			addIfUnique(set, simplex.d);
			MANI_TEST_ASSERT(set.size() == 4, "all simplex points must be unique");
		}

		MANI_TEST(SimpleYesCollisionWithRotation, "simple case with collision, with rotation")
		{
			std::vector<glm::vec3> shape1 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};
			Transform transform1;
			transform1.position = glm::vec3(1.f);
			transform1.rotation = glm::angleAxis(glm::radians(45.f), glm::vec3(1.f, 0.f, 0.f));

			std::vector<glm::vec3> shape2 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};

			Transform transform2;
			transform2.position = glm::vec3(1.5f);

			Simplex simplex;
			const bool result = GJK::solve(shape1, &transform1, shape2, &transform2, simplex);
			MANI_TEST_ASSERT(result == true, "Shape are colliding");

			std::vector<glm::vec3> set;
			addIfUnique(set, simplex.a);
			addIfUnique(set, simplex.b);
			addIfUnique(set, simplex.c);
			addIfUnique(set, simplex.d);
			MANI_TEST_ASSERT(set.size() == 4, "all simplex points must be unique");
		}
	}
	MANI_SECTION_END(GJKTest)

	MANI_SECTION_BEGIN(EPATest, "EPA")
	{
		/*MANI_TEST(SimpleWithCollisionEPA, "Simple case with collision")
		{
			std::vector<glm::vec3> shape1 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};
			Transform transform1;
			transform1.position = glm::vec3(1.f);

			std::vector<glm::vec3> shape2 = {
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.f, 0.f },
				{ 1.f, 1.f, 0.f },
				{ 0.f, 1.f, 0.f },

				{ 0.f, 0.f, 1.f },
				{ 1.f, 0.f, 1.f },
				{ 1.f, 1.f, 1.f },
				{ 0.f, 1.f, 1.f }
			};

			Transform transform2;
			transform2.position = glm::vec3(1.5f);

			Simplex simplex;
			const bool GJKResult = GJK::solve(shape1, &transform1, shape2, &transform2, simplex);
			MANI_TEST_ASSERT(GJKResult == true, "Shape are colliding");

			HitResult hitResult;
			const bool EPAResult = EPA::solve(shape1, &transform1, shape2, &transform2, simplex, hitResult);
			MANI_TEST_ASSERT(EPAResult == true, "Shape are penetrating");
			MANI_TEST_ASSERT(hitResult.isHit == true, "it is a hit");
			MANI_TEST_ASSERT(glm::length2(hitResult.normal - glm::normalize((transform2.position - transform1.position))) < FLT_EPSILON, "it is penetrating in the correct direction");
			MANI_TEST_ASSERT(hitResult.penetrationDepth > FLT_EPSILON, "the penetration depth is by the correct amount");
		}*//*
	}
	MANI_SECTION_END(EPATest)
}
MANI_SECTION_END(Collision)
*/