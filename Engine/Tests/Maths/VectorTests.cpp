#include "simpleTests.h"
#include <Maths/Vector.h>
#include <Core/Log.h>

using namespace ECSEngine;

ST_SECTION_BEGIN(Maths, "Maths")
{
	ST_SECTION_BEGIN(Vector, "Vector")
	{
		ST_TEST(VectorBasics, "Should execute basic vector operations")
		{
			Vector3 vector;
			ST_ASSERT(
				vector.x <= FLT_EPSILON &&
				vector.y <= FLT_EPSILON &&
				vector.z <= FLT_EPSILON,
				"Should initialize a vector3."
			);

			vector.x += 1.f;
			ST_ASSERT((vector.x - 1.f) <= FLT_EPSILON, "vector.x should be 1.f");
			
			Vector3 result;
			
			result = vector + Vector3{ 0.f, 2.f, 3.f };
			ST_ASSERT(
				result.x - 1.f <= FLT_EPSILON &&
				result.y - 2.f <= FLT_EPSILON &&
				result.z - 3.f <= FLT_EPSILON,
				"Should be the proper result."
			);

			result = Vector3{ 1.f, 2.f, 3.f } - Vector3{ 1.f, 2.f, 3.f };
			ST_ASSERT(
				result.x <= FLT_EPSILON &&
				result.y <= FLT_EPSILON &&
				result.z <= FLT_EPSILON,
				"Should be the proper result."
			);

			result = Vector3{ 1.f, 2.f, 3.f } * 2.f;
			ST_ASSERT(
				result.x - 2.f <= FLT_EPSILON &&
				result.y - 4.f <= FLT_EPSILON &&
				result.z - 6.f <= FLT_EPSILON,
				"Should be the proper result."
			);

			result = Vector3{ 1.f, 2.f, 3.f } / 2.f;
			ST_ASSERT(
				result.x - .5f <= FLT_EPSILON &&
				result.y - 1.f <= FLT_EPSILON &&
				result.z - 1.5f <= FLT_EPSILON,
				"Should be the proper result."
			);

			ST_ASSERT((Vector3{ 1.f, 2.f, 3.f } - Vector3{ 1.f, 2.f, 3.f }).isNearlyZero(), "Should be nearly zero");

			ST_ASSERT(std::abs(Vector3::dot(Vector3::RIGHT, Vector3::FORWARD)) <= FLT_EPSILON, "Should be 0.f");
			ST_ASSERT((Vector3::cross(Vector3::RIGHT, Vector3::UP) - Vector3::FORWARD).isNearlyZero(), "Should be 0.f");
		}

		ST_TEST(VectorInitializations, "should be able to init a vector.")
		{
			Vector3 vector3{ 1.f, 2.f, 3.f };
			ST_ASSERT(
				vector3.x - 1.f<= FLT_EPSILON &&
				vector3.y - 2.f<= FLT_EPSILON &&
				vector3.z - 3.f<= FLT_EPSILON,
				"Should initialize a vector3."
			);
		}
	}
	ST_SECTION_END(Vector)
}
ST_SECTION_END(Maths)
