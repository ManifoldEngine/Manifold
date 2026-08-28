#pragma once

#include <Core/ManiTypes.h>
#include <array>
#include <algorithm>

namespace Mani
{
	template<typename T, SizeT Size>
	class Array
	{
	public:
		using Iterator = typename std::array<T, Size>::iterator;
		using IteratorConst = typename std::array<T, Size>::const_iterator;

		using Predicate = bool(const T&);

		// STL compliance
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = SizeT;

		// constructors
		Array() = default;
		Array(const std::initializer_list<T>& initializerList)
		{
			MANI_ASSERT(initializerList.size() == Size, "Initializer list size mismatch");
			std::copy(initializerList.begin(), initializerList.end(), m_data.begin());
		}
		Array(const Array<T, Size>& other) : m_data(other.m_data) {}
		Array(Array<T, Size>&& other) noexcept : m_data(std::move(other.m_data)) {}

		Array<T, Size>& operator=(const Array<T, Size>& other)
		{
			if (this != &other)
			{
				m_data = other.m_data;
			}
			return *this;
		}

		Array<T, Size>& operator=(Array<T, Size>&& other) noexcept
		{
			if (this != &other)
			{
				m_data = std::move(other.m_data);
			}
			return *this;
		}

		// capacity
		[[nodiscard]] SizeT size() const { return Size; }
		
		// mutators
		void fill(const T& value)
		{
			for (SizeT i = 0; i < Size; ++i)
			{ 
				m_data[i] = value;
			}
		}

		// algo
		[[nodiscard]] bool contains(const T& value) const
		{
			return std::find(m_data.begin(), m_data.end(), value) != m_data.end();
		}

		template<typename F = Predicate>
		[[nodiscard]] bool containsIf(F&& f) const
		{
			return std::find_if(m_data.begin(), m_data.end(), f) != m_data.end();
		}

		template<typename F = Predicate>
		[[nodiscard]] T* findIf(F&& f)
		{
			auto it = std::find_if(m_data.begin(), m_data.end(), f);
			if (it != m_data.end())
			{
				return &(*it);
			}
			return nullptr;
		}

		template<typename F = Predicate>
		[[nodiscard]] const T* findIf(F&& f) const
		{
			auto it = std::find_if(m_data.begin(), m_data.end(), f);
			if (it != m_data.end())
			{
				return &(*it);
			}
			return nullptr;
		}

		[[nodiscard]] SizeT indexOf(const T& value) const
		{
			auto it = std::find(m_data.begin(), m_data.end(), value);
			if (it == m_data.end())
			{
				return INDEX_NONE;
			}
			return static_cast<SizeT>(it - m_data.begin());
		}

		template<typename F = Predicate>
		[[nodiscard]] SizeT indexOfIf(F&& f) const
		{
			auto it = std::find_if(m_data.begin(), m_data.end(), f);
			if (it == m_data.end())
			{
				return INDEX_NONE;
			}
			return static_cast<SizeT>(it - m_data.begin());
		}

		void sort()
		{
			std::sort(m_data.begin(), m_data.end());
		}

		[[nodiscard]] Array<T, Size> sortCopy() const
		{
			Array<T, Size> copy = *this;
			std::sort(copy.begin(), copy.end());
			return copy;
		}

		template<typename TPredicate = bool(const T&, const T&)>
		void sort(TPredicate&& f)
		{
			std::sort(m_data.begin(), m_data.end(), [&f](const T& lhs, const T& rhs)
			{
				return f(lhs, rhs);
			});
		}

		template<typename TPredicate = bool(const T&, const T&)>
		[[nodiscard]] Array<T, Size> sortCopy(TPredicate&& f) const
		{
			Array<T, Size> copy = *this;
			std::sort(copy.begin(), copy.end(), [&f](const T& lhs, const T& rhs)
			{
				return f(lhs, rhs);
			});
			return copy;
		}

		// access
		[[nodiscard]] bool isValid(SizeT index) const { return index < m_data.size(); }
		[[nodiscard]] T& operator[](SizeT index) { MANI_ASSERT(isValid(index), "Out of bounds."); return m_data[index]; }
		[[nodiscard]] const T& operator[](SizeT index) const { MANI_ASSERT(isValid(index), "Out of bounds."); return m_data[index]; }
		[[nodiscard]] T& first() { return m_data.front(); }
		[[nodiscard]] const T& first() const { return m_data.front(); }
		[[nodiscard]] T& last() { return m_data.back(); }
		[[nodiscard]] const T& last() const { return m_data.back(); }

		[[nodiscard]] const T& at(SizeT index) const
		{
			MANI_ASSERT(isValid(index), "Out of bounds.");
			return m_data[index];
		}

		[[nodiscard]] T& at(SizeT index)
		{
			MANI_ASSERT(isValid(index), "Out of bounds.");
			return m_data[index];
		}

		[[nodiscard]] std::array<T, Size>& data() { return m_data; }
		[[nodiscard]] const std::array<T, Size>& data() const { return m_data; }

		// iterators
		Iterator begin() { return m_data.begin(); }
		IteratorConst begin() const { return m_data.begin(); }
		Iterator end() { return m_data.end(); }
		IteratorConst end() const { return m_data.end(); }

		// comparison

		bool operator==(const Mani::Array<T, Size>& other) const
		{
			return m_data == other.m_data;
		}

		bool operator!=(const Mani::Array<T, Size>& other) const
		{
			return m_data != other.m_data;
		}

	private:
		std::array<T, Size> m_data;
	};
}