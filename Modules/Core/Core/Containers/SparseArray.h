#pragma once

#include <Core/Containers/Array.h>
#include <Core/Containers/PagedList.h>
#include <Core/ManiTraits.h>
#include <ManiMaths/Traits.h>

#if MANI_DEBUG
#include <ManiZ/Reflection.h>
#endif

namespace Mani
{
	template<typename TIndex, SizeT SIZE>
	requires(IsInteger<TIndex>)
	class ISparseArray
	{
	public:
		virtual TIndex bound() const = 0;
		virtual TIndex count() const = 0;
		virtual bool isEmpty() const = 0;
		virtual bool unset(TIndex index) = 0;
		virtual const Mani::Array<TIndex, SIZE>& getDenseIndices() const = 0;

#if MANI_DEBUG
		virtual std::string_view debug_getName() const = 0;
#endif
	};

	template<typename T, typename TIndex, SizeT SIZE>
	requires(IsUnsignedInteger<TIndex>)
	class SparseArray : public ISparseArray<TIndex, SIZE>
	{
	public:
		inline static constexpr TIndex PAGE_SIZE = 65536; // 2^16
		inline static constexpr TIndex INDEX_NONE = std::numeric_limits<TIndex>::max();

		SparseArray() = default;

		[[nodiscard]] TIndex bound() const { return m_bound; }
		[[nodiscard]] TIndex count() const { return m_bound - static_cast<TIndex>(m_free.count()); }
		[[nodiscard]] bool isEmpty() const { return m_bound == static_cast<TIndex>(m_free.count()); }
		
		// sets at index, forwards value to the dense slot
		void set(TIndex index, T&& value)
		{
			TIndex denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				denseIndex = !m_free.isEmpty() ? m_free.pop() : m_bound++;
				m_sparse.insert(index, denseIndex);
				m_denseIndices[denseIndex] = index;
			}
			T* buffer = &m_dense[denseIndex];
			new (buffer) T(std::forward<T>(value));
		}

		// returns a stable ref at index
		[[nodiscard]] T& get(TIndex index)
		{
			return m_dense[m_sparse.get(index)];
		}

		// return a stable pointer at index
		[[nodiscard]] T* getPtr(TIndex index)
		{
			const TIndex denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				return nullptr;
			}
			return &m_dense[denseIndex];
		}

		// returns a stable ref at index
		[[nodiscard]] const T& get(TIndex index) const
		{
			return m_dense[m_sparse.get(index)];
		}

		// return a stable pointer at index
		[[nodiscard]] const T* getPtr(TIndex index) const
		{
			const TIndex denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				return nullptr;
			}
			return &m_dense[denseIndex];
		}

		// unsets at index, calls destructor
		bool unset(TIndex index) 
		{
			if (isEmpty())
			{
				return false;
			}

			const TIndex denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				return false;
			}

			T& value = m_dense[denseIndex];
			value.~T();
			m_denseIndices[denseIndex] = INDEX_NONE;
			m_sparse.removeAt(index);
			m_free.add(denseIndex);
			return true;
		}

		Mani::Array<T, SIZE>& getDense()
		{
			return m_dense;
		}

		const Mani::Array<T, SIZE>& getDense() const
		{
			return m_dense;
		}
		
		const Mani::Array<TIndex, SIZE>& getDenseIndices() const
		{
			return m_denseIndices;
		}

		TIndex toDenseIndex(TIndex index) const
		{
			return m_sparse.safeGet(index).getOr(INDEX_NONE);
		}

	private:
#if MANI_DEBUG
	public:
		std::string_view debug_getName() const
		{
			return debug_name;
		}

		void debug_overrideName(const std::string_view& name)
		{
			debug_name = name;
		}

	private:
		// storing the name of T so it's visible in the debugger. 
		std::string_view debug_name = ManiZ::RFL::getTypeName<T>();
#endif
		Mani::PagedList<TIndex, PAGE_SIZE> m_sparse;
		Mani::Array<T, SIZE> m_dense;
		Mani::Array<TIndex, SIZE> m_denseIndices;
		TIndex m_bound = 0;
		Mani::List<TIndex> m_free;
	};
}