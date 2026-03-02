#pragma once

#include <Core/ManiAssert.h>
#include <Core/Containers/List.h>
#include <Core/Containers/Array.h>
#include <Core/Containers/Optional.h>
#include <Core/ManiMaths.h>
#include <memory>

namespace Mani
{
	template<typename T, SizeT PageSize>
	requires(PageSize > 0)
	class PagedList
	{
		inline static constexpr SizeT INITIAL_PAGE_COUNT = 1;

		using Line = Mani::Optional<T>;
		using Page = Mani::Array<Line, PageSize>;
		using PagePtr = std::shared_ptr<Page>;
		using Pages = Mani::List<PagePtr>;

	public:
		PagedList() : m_pages(std::make_unique<Pages>(INITIAL_PAGE_COUNT))
		{
			Pages& pages = *m_pages.get();
			for (SizeT i = 0; i < INITIAL_PAGE_COUNT; i++)
			{
				pages[i] = std::make_shared<Page>();
			}
		}

		void insert(SizeT index, const T& element)
		{
			Page& page = getOrCreatePage(index);
			const SizeT pageIndex = toPageIndex(index);
			page[pageIndex] = element;
		}

		[[nodiscard]] T& get(SizeT index)
		{
			Line& line = getPage(index)[toPageIndex(index)];
			MANI_ASSERT(line.isSet(), "Trying to get on an unset line.");
			return line.get();
		}

		[[nodiscard]] const T& get(SizeT index) const
		{
			const Line& line = getPage(index)[toPageIndex(index)];
			MANI_ASSERT(line.isSet(), "Trying to get on an unset line.");
			return line.get();
		}

		[[nodiscard]] Mani::Optional<T> safeGet(SizeT index) const
		{
			const SizeT page = toPage(index);
			if (!m_pages->isValid(page))
			{
				return Mani::Optional<T>();
			}

			return m_pages->at(page)->at(toPageIndex(index));
		}

		bool removeAt(SizeT index)
		{
			const SizeT page = toPage(index);
			if (!m_pages->isValid(page))
			{
				return false;
			}
			Line& line = m_pages->at(page)->at(toPageIndex(index));
			const bool didRemove = line.isSet();
			line.unset();
			return didRemove;
		}

		void clear()
		{
			m_pages->clear();
		}

	private:
		[[nodiscard]] static SizeT toPage(SizeT index)
		{
			return index / PageSize;
		}

		[[nodiscard]] static SizeT toPageIndex(SizeT index)
		{
			return index % PageSize;
		}

		[[nodiscard]] Page& getOrCreatePage(SizeT index)
		{
			const SizeT page = toPage(index);
			if (!m_pages->isValid(page))
			{
				resize(page + 1);
			}

			Pages& pages = *m_pages.get();

			if (pages[page] == nullptr)
			{
				pages[page] = std::make_shared<Page>();
			}

			return *(pages[page].get());
		}

		[[nodiscard]] Page& getPage(SizeT index)
		{
			const SizeT page = toPage(index);
			MANI_ASSERT(m_pages->isValid(page), "Trying to get an invalid page.");
			Page* pagePtr = m_pages->at(page).get();
			MANI_ASSERT(pagePtr != nullptr, "Trying to get a uninitialized page.");
			return *pagePtr;
		}

		void resize(SizeT newSize)
		{
			std::unique_ptr<Pages> newPages = std::make_unique<Pages>(newSize);
			const SizeT smallest = Mani::Math::minT(newSize, m_pages->count());
			for (SizeT i = 0; i < smallest; i++)
			{
				if (m_pages->at(i) != nullptr)
				{
					newPages->at(i) = std::move(m_pages->at(i));
				}
			}
			m_pages.reset();
			m_pages = std::move(newPages);
		}

		std::unique_ptr<Pages> m_pages;
	};
}