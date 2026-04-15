#pragma once

#include <Core/Vec.h>

namespace Mani
{
	struct Scale : public Vec3f 
	{
		constexpr Scale() : Vec3f(VEC3F::ONE) {}
		constexpr Scale(const Vec3f& v) : Vec3f(v) {};
		constexpr Scale(Vec3f&& v) : Vec3f(std::forward<Vec3f>(v)) {};
		constexpr Scale(float x, float y, float z) : Vec3f(x, y, z) {};

		constexpr Scale& operator=(const Vec3f& v)
		{
			Vec3f::operator=(v);
			return *this;
		}

		constexpr Scale& operator=(Vec3f&& v) noexcept
		{
			Vec3f::operator=(std::move(v));
			return *this;
		}
	};
}