#pragma once

#include <limits>

namespace Mani
{
	using SizeT = std::size_t;
	inline constexpr SizeT INDEX_NONE = (std::numeric_limits<SizeT>::max)();
}