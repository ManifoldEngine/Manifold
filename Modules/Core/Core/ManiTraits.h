#pragma once

#include <type_traits>

namespace Mani
{
	template<class T, class U>
	concept DerivedFrom = std::is_base_of<U, T>::value;

	template<typename U, typename V>
	constexpr bool isDerivedFrom(const V& value)
	{
		if constexpr (std::is_base_of<V, U>::value)
		{
			return true;
		}
		return false;
	}

	template<typename... Ts>
	struct TypeList {};
}