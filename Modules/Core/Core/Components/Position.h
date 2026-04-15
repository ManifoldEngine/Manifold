#pragma once

#include <Core/Vec.h>

namespace Mani
{
	struct Position : Vec3f 
	{
		constexpr Position() : Vec3f(VEC3F::ZERO) {}
		constexpr Position(const Vec3f& v) : Vec3f(v) {};
		constexpr Position(Vec3f&& v) : Vec3f(std::forward<Vec3f>(v)) {};
		constexpr Position(float x, float y, float z) : Vec3f(x, y, z) {};

		constexpr Position& operator=(const Vec3f& v)
		{
			Vec3f::operator=(v);
			return *this;
		}

		constexpr Position& operator=(Vec3f&& v) noexcept
		{
			Vec3f::operator=(std::move(v));
			return *this;
		}
	};
}