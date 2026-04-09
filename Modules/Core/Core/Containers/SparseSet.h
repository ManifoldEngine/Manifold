#pragma once

#include <Core/Containers/List.h>
#include <Core/Containers/PagedList.h>
#include <Core/ManiTraits.h>
#include <ManiMaths/Traits.h>

#if MANI_DEBUG
#include <ManiZ/Reflection.h>
#endif

namespace Mani
{
	class ISparseSet
	{
	public:
		virtual SizeT count() const = 0;
		virtual bool isEmpty() const = 0;
		virtual SizeT removeSwap(SizeT index) = 0;
		virtual const Mani::List<SizeT>& getDenseIndices() const = 0;

#if MANI_DEBUG
		virtual std::string_view debug_getName() const = 0;
#endif
	};

	template<typename T, typename TIndex = SizeT>
	requires(IsUnsignedInteger<TIndex> && Is8BytesType<TIndex>)
	class SparseSet : public ISparseSet
	{
	public:
		inline static constexpr SizeT PAGE_SIZE = 2048;

		SparseSet() = default;

		SparseSet(SizeT capacity)
		{
			m_dense.reserve(capacity);
			m_denseIndices.reserve(capacity);
		}

		[[nodiscard]] SizeT count() const { return m_dense.count(); }
		[[nodiscard]] bool isEmpty() const { return m_dense.isEmpty(); }

		// inserts value at index
		void insert(TIndex index, T&& value)
		{
			SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				denseIndex = m_dense.count();
				m_sparse.insert(index, denseIndex);
				m_dense.insert(denseIndex, std::forward<T>(value));
				m_denseIndices.add(index);
			}
			else
			{
				m_dense[denseIndex] = std::forward<T>(value);
			}
		}

		// returns a ref at index
		[[nodiscard]] T& get(TIndex index)
		{
			return m_dense[m_sparse.get(index)];
		}

		// return a pointer at index
		[[nodiscard]] T* getPtr(TIndex index)
		{
			const SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				return nullptr;
			}
			return &m_dense[denseIndex];
		}

		// returns a ref at index
		[[nodiscard]] const T& get(TIndex index) const
		{
			return m_dense[m_sparse.get(index)];
		}

		// return a pointer at index
		[[nodiscard]] const T* getPtr(TIndex index) const
		{
			const SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				return nullptr;
			}
			return &m_dense[denseIndex];
		}

		// remove swaps at the index from the dense array
		// returns the new index that takes its place or INDEX_NONE if it was the last
		SizeT removeSwap(TIndex index)
		{
			if (m_dense.isEmpty())
			{
				return INDEX_NONE;
			}

			const SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				return INDEX_NONE;
			}

			SizeT swappedIndex = m_denseIndices.last();
			if (swappedIndex == index)
			{
				// we're removing the last index, so we set the swapped index to none
				swappedIndex = INDEX_NONE;
			}

			m_dense.removeSwapAt(denseIndex);
			m_denseIndices.removeSwapAt(denseIndex);

			m_sparse.removeAt(index);
			if (swappedIndex != INDEX_NONE)
			{
				m_sparse.insert(swappedIndex, denseIndex);
			}

			return swappedIndex;
		}

		// returns the dense data
		Mani::List<T>& getDense()
		{
			return m_dense;
		}

		// returns the dense data
		const Mani::List<T>& getDense() const
		{
			return m_dense;
		}

		// returns the dense data indices
		const Mani::List<TIndex>& getDenseIndices() const
		{
			return m_denseIndices;
		}

		// transforms an index to its dense counterpart
		SizeT toDenseIndex(TIndex index) const
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
		Mani::PagedList<SizeT, PAGE_SIZE> m_sparse;
		Mani::List<T> m_dense;
		Mani::List<TIndex> m_denseIndices;
	};
}
