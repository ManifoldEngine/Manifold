#pragma once

#include <Core/ManiAssert.h>
#include <Core/ManiTypes.h>
#include <vector>
#include <algorithm>

namespace Mani
{
	template<typename T>
	class List
	{
	public:
		using Iterator = typename std::vector<T>::iterator;
		using IteratorConst = typename std::vector<T>::const_iterator;
		using ReverseIterator = typename std::vector<T>::reverse_iterator;
		using ReverseIteratorConst = typename std::vector<T>::const_reverse_iterator;

		using Predicate = bool(const T&);

		// STL compliance
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = SizeT;

		// constructors
		List() = default;
		List(SizeT count, const T& value = T{}) : m_data(count, value) {}
		List(const std::initializer_list<T>& initializerList) : m_data(initializerList) {}
		List(const List<T>& other) : m_data(other.m_data) {}
		List(List<T>&& other) noexcept : m_data(std::move(other.m_data)) {}

		List<T>& operator=(const List<T>& other)
		{
			if (this != &other)
			{
				m_data = other.m_data;
			}
			return *this;
		}

		List<T>& operator=(List<T>&& other) noexcept
		{
			if (this != &other)
			{
				m_data = std::move(other.m_data);
			}
			return *this;
		}

		// capacity
		[[nodiscard]] SizeT count() const { return m_data.size(); }
		[[nodiscard]] SizeT capacity() const { return m_data.capacity(); }
		[[nodiscard]] bool isEmpty() const { return m_data.empty(); }
		void reserve(SizeT inCapacity) { m_data.reserve(inCapacity); }
		void shrink() { m_data.shrink_to_fit(); }
		void resize(SizeT newSize) { m_data.resize(newSize); }
		void resize(SizeT newSize, T&& fillValue) { m_data.resize(newSize, fillValue); }

		// mutators
		void clear() { m_data.clear(); }
		void add(const T& value) { m_data.push_back(value); }
		void add(T&& value) { m_data.emplace_back(std::forward<T>(value)); }
		void enqueue(const T& value) { m_data.insert(m_data.begin(), value); }
		void enqueue(T&& value) { m_data.emplace(m_data.begin(), std::forward<T>(value)); }
		
		void append(const List<T>& other)
		{
			m_data.insert(m_data.end(), other.m_data.begin(), other.m_data.end());
		}
		
		void insert(SizeT index, const T& value)
		{
			m_data.insert(m_data.begin() + index, value);
		}

		void insert(SizeT index, T&& value)
		{
			m_data.emplace(m_data.begin() + index, std::forward<T>(value));
		}

		bool remove(const T& value)
		{
			auto it = std::find(m_data.begin(), m_data.end(), value);
			return it != m_data.end() && (m_data.erase(it), true);
		}

		bool removeSwap(const T& value)
		{
			return removeSwapAt(indexOf(value));
		}

		bool removeSwapAt(SizeT index)
		{
			if (index >= m_data.size())
			{
				return false;
			}
			
			m_data[index] = std::move(m_data.back());
			m_data.pop_back();
			return true;
		}

		bool removeAll(const T& value)
		{
			const std::size_t old = m_data.size();
			m_data.erase(std::remove(m_data.begin(), m_data.end()), m_data.end());
			return old != m_data.size();
		}

		T dequeue()
		{
			T value = std::move(*(m_data.begin()));
			m_data.erase(m_data.begin());
			return value;
		}

		void swap(SizeT i1, SizeT i2)
		{
			std::swap(m_data[i1], m_data[i2]);
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

		void addUnique(const T& value)
		{
			if (contains(value))
			{
				return;
			}

			add(value);
		}

		void addUnique(T&& value)
		{
			if (contains(value))
			{
				return;
			}

			add(std::forward<T>(value));
		}

		template<typename F = Predicate>
		bool removeIf(F&& f)
		{
			const std::size_t old = m_data.size();
			auto it = m_data.erase(std::remove_if(m_data.begin(), m_data.end(), f), m_data.end());
			return old != m_data.size();
		}

		void removeAt(SizeT index)
		{
			MANI_ASSERT(isValid(index), "Invalid index");
			const auto it = m_data.begin() + index;
			m_data.erase(it);
		}

		void removeLast()
		{
			m_data.pop_back();
		}

		T pop()
		{
			T value = std::move(last());
			m_data.pop_back();
			return value;
		}

		void sort()
		{
			std::sort(m_data.begin(), m_data.end());
		}

		[[nodiscard]] List<T> sortCopy() const
		{
			List<T> copy = *this;
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
		[[nodiscard]] List<T> sortCopy(TPredicate&& f) const
		{
			List<T> copy = *this;
			std::sort(copy.begin(), copy.end(), [&f](const T& lhs, const T& rhs)
			{
				return f(lhs, rhs);
			});
			return copy;
		}

		// access
		[[nodiscard]] bool isValid(SizeT index) const { return index < m_data.size(); }
		[[nodiscard]] T& operator[](SizeT index) { MANI_ASSERT(isValid(index), "Out of bounds"); return m_data[index]; }
		[[nodiscard]] const T& operator[](SizeT index) const { MANI_ASSERT(isValid(index), "Out of bounds"); return m_data[index]; }
		[[nodiscard]] T& first() { return m_data.front(); }
		[[nodiscard]] const T& first() const { return m_data.front(); }
		[[nodiscard]] T& last() { return m_data.back(); }
		[[nodiscard]] const T& last() const { return m_data.back(); }

		[[nodiscard]] T* firstPtr()
		{
			if (isEmpty())
			{
				return nullptr;
			}
			return &m_data.front();
		}

		[[nodiscard]] const T* firstPtr() const
		{
			if (isEmpty())
			{
				return nullptr;
			}
			return &m_data.front();
		}

		[[nodiscard]] T* lastPtr()
		{
			if (isEmpty())
			{
				return nullptr;
			}
			return &m_data.back();
		}

		[[nodiscard]] const T* lastPtr() const
		{
			if (isEmpty())
			{
				return nullptr;
			}
			return &m_data.back();
		}

		[[nodiscard]] T& at(SizeT index)
		{
			MANI_ASSERT(isValid(index), "Out of bounds");
			return m_data[index];
		}

		[[nodiscard]] const T& at(SizeT index) const
		{
			MANI_ASSERT(isValid(index), "Out of bounds");
			return m_data[index];
		}

		[[nodiscard]] T* atPtr(SizeT index)
		{
			if (!isValid(index))
			{
				return nullptr;
			}
			return &m_data[index];
		}

		[[nodiscard]] const T* atPtr(SizeT index) const
		{
			if (!isValid(index))
			{
				return nullptr;
			}
			return &m_data[index];
		}

		[[nodiscard]] T* data() { return m_data.data(); }
		[[nodiscard]] const T* data() const { return m_data.data(); }

		// iterators
		Iterator begin() { return m_data.begin(); }
		IteratorConst begin() const { return m_data.begin(); }
		Iterator end() { return m_data.end(); }
		IteratorConst end() const { return m_data.end(); }

		ReverseIterator rbegin() { return m_data.rbegin(); }
		ReverseIteratorConst rbegin() const { return m_data.rbegin(); }
		ReverseIterator rend() { return m_data.rend(); }
		ReverseIteratorConst rend() const { return m_data.rend(); }
		
		// comparison
		bool operator==(const Mani::List<T>& other) const
		{
			return m_data == other.m_data;
		}

		bool operator!=(const Mani::List<T>& other) const
		{
			return m_data != other.m_data;
		}
	
	private:
		std::vector<T> m_data;
	};
}