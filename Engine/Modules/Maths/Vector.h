#pragma once

#include <algorithm>
#include <Core/Assert.h>

namespace ECSEngine
{
	struct Vector3
	{
		Vector3() 
			: x(ZERO.x), y(ZERO.y), z(ZERO.z)
		{}
		Vector3(float inX, float inY, float inZ)
			: x(inX), y(inY), z(inZ)
		{}
		
		Vector3 operator+(const Vector3& other)
		{
			return Vector3(
				x + other.x,
				y + other.y,
				z + other.z
			);
		}

		Vector3 operator-(const Vector3& other)
		{
			return Vector3(
				x - other.x,
				y - other.y,
				z - other.z
			);
		}

		Vector3 operator*(const float c)
		{
			return Vector3(
				x * c,
				y * c,
				z * c
			);
		}

		Vector3 operator/(const float c)
		{
			ECSE_ASSERT(std::abs(c), "Dividing by zero!");
			return Vector3(
				x / c,
				y / c,
				z / c
			);
		}

		bool isNearlyZero(float tolerance = FLT_EPSILON) const
		{
			return	std::abs(x) <= tolerance &&
					std::abs(y) <= tolerance &&
					std::abs(z) <= tolerance;
		}

		static float dot(const Vector3& v1, const Vector3& v2)
		{
			return (v1.x * v2.x) +
				(v1.y * v2.y) +
				(v1.z * v2.z);
		}
		
		static Vector3 cross(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(
				(v1.y * v2.z - v1.z * v2.y),
				(v1.z * v2.x - v1.x * v2.z),
				(v1.x * v2.y - v1.y * v2.x)
			);
		}

		float x;
		float y;
		float z;

		const static Vector3 ZERO;
		const static Vector3 UP;
		const static Vector3 DOWN;
		const static Vector3 RIGHT;
		const static Vector3 LEFT;
		const static Vector3 FORWARD;
		const static Vector3 BACK;
	};
}