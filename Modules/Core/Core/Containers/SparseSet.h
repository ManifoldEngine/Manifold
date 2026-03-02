#pragma once

#include <Core/ECS/Entity.h>
#include <Core/Containers/List.h>
#include <Core/Containers/PagedList.h>

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
		virtual bool remove(SizeT index) = 0;
		virtual const Mani::List<SizeT>& getDenseIndices() const = 0;

#if MANI_DEBUG
		virtual std::string_view debug_getTypeName() const = 0;
#endif
	};

	template<typename T>
	class SparseSet : public ISparseSet
	{
		inline static constexpr SizeT PAGE_SIZE = 2048;
		inline static constexpr SizeT INITIAL_CAPACITY = 1000;
		inline static constexpr SizeT UNSET_INDEX = INDEX_NONE;

	public:
		SparseSet()
		{
			m_dense.reserve(INITIAL_CAPACITY);
			m_denseIndices.reserve(INITIAL_CAPACITY);
		}

		[[nodiscard]] SizeT count() const { return m_dense.count(); }
		[[nodiscard]] bool isEmpty() const { return m_dense.isEmpty(); }

		[[nodiscard]] T& set(SizeT index)
		{
			SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				denseIndex = m_dense.count();
				m_sparse.insert(index, denseIndex);
				m_dense.add(T{});
				m_denseIndices.add(index);
			}
			return m_dense[denseIndex];
		}

		[[nodiscard]] T& get(SizeT index)
		{
			return m_dense[m_sparse.get(index)];
		}

		[[nodiscard]] T* getPtr(SizeT index)
		{
			const SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == INDEX_NONE)
			{
				return nullptr;
			}
			return &m_dense[denseIndex];
		}

		[[nodiscard]] const T& get(SizeT index) const
		{
			return m_dense[m_sparse.get(index)];
		}

		[[nodiscard]] const T* getPtr(SizeT index) const
		{
			const SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == UNSET_INDEX)
			{
				return nullptr;
			}
			return &m_dense[denseIndex];
		}

		bool remove(SizeT index)
		{
			if (m_dense.isEmpty())
			{
				return false;
			}

			const SizeT denseIndex = toDenseIndex(index);
			if (denseIndex == UNSET_INDEX)
			{
				return false;
			}

			const SizeT lastIndex = m_denseIndices.last();

			m_dense.removeSwapAt(denseIndex);
			m_denseIndices.removeSwapAt(denseIndex);

			m_sparse.removeAt(index);
			if (!m_dense.isEmpty())
			{
				m_sparse.insert(lastIndex, denseIndex);
			}

			return true;
		}

		Mani::List<T>& getDense()
		{
			return m_dense;
		}

		const Mani::List<T>& getDense() const
		{
			return m_dense;
		}

		const Mani::List<SizeT>& getDenseIndices() const
		{
			return m_denseIndices;
		}

		SizeT toDenseIndex(SizeT index) const
		{
			return m_sparse.safeGet(index).getOr(UNSET_INDEX);
		}

	private:

#if MANI_DEBUG
	public:
		std::string_view debug_getTypeName() const
		{
			return ManiZ::RFL::getTypeName<T>();
		}

		// storing the name as the so it's visible in the debugger. 
		std::string_view debug_typeName = ManiZ::RFL::getTypeName<T>();
	private:
#endif

		Mani::PagedList<SizeT, PAGE_SIZE> m_sparse;
		Mani::List<T> m_dense;
		Mani::List<SizeT> m_denseIndices;
	};
}
