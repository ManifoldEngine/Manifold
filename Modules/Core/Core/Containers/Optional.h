#pragma once

#include <optional>
#include <type_traits>

namespace Mani
{
	template<class T>
	class Optional 
	{
	public:
		constexpr Optional() = default;

		template<class... Ts, std::enable_if_t<std::is_constructible_v<T, Ts...>, int> = 0>
		constexpr Optional(Ts&&... args) : m_data(args...)
		{
		}

		constexpr bool isSet() const
		{
			return m_data.has_value();
		}

		constexpr void set(T&& value)
		{
			m_data = std::optional<T>(std::forward<T>(value));
		}

		constexpr void unset()
		{
			m_data = std::optional<T>();
		}

		constexpr T& get()
		{
			return m_data.value();
		}

		constexpr const T& get() const
		{
			return m_data.value();
		}

		template<class T2 = std::remove_cv_t<T>>
		requires(std::is_convertible_v<T2, std::remove_cv_t<T>>)
		constexpr std::remove_cv_t<T> getOr(T2&& value)
		{
			return m_data.value_or(value);
		}

	private:
		std::optional<T> m_data;
	};
}