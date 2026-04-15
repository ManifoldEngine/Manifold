#pragma once

#include <ManiMaths/Quat.h>

namespace Mani
{
	struct Rotation : public Quatf 
	{
		constexpr Rotation() : Quatf(QUATF::IDENTITY) {}
		constexpr Rotation(const Quatf& v) : Quatf(v) {};
		constexpr Rotation(Quatf&& v) : Quatf(std::forward<Quatf>(v)) {};
		constexpr Rotation(float x, float y, float z, float w) : Quatf(x, y, z, w) {};

		constexpr Rotation& operator=(const Quatf& v)
		{
			Quatf::operator=(v);
			return *this;
		}

		constexpr Rotation& operator=(Quatf&& v) noexcept
		{
			Quatf::operator=(std::move(v));
			return *this;
		}
	};
}