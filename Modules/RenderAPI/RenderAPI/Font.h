#pragma once

#include <Core/Vec.h>

namespace Mani
{
	struct Font
	{
		struct Glyph
		{
			Mani::Vec2f size = Mani::VEC2F::ZERO;
			Mani::Vec2f bearing = Mani::VEC2F::ZERO;
			float advance = 0.f;

			Mani::Vec2f uvMin = Mani::VEC2F::ZERO;
			Mani::Vec2f uvMax = Mani::VEC2F::ZERO;
		};

		unsigned char* buffer = nullptr;
	};
}