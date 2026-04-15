#pragma once

#include <type_traits>

namespace Mani
{
	using Dtor = void(*)(const void*);

	template<class T, class U>
	concept DerivedFrom = std::is_base_of<U, T>::value;

	template <class T>
	concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

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

	template<typename T>
	using Bare = std::remove_cvref_t<T>;
}