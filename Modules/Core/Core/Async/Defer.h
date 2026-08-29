#pragma once

#include <Core/Containers/List.h>
#include <Core/Containers/Optional.h>
#include <functional>
#include <memory>
#include <type_traits>

namespace Mani
{
	class Deferred
	{
	public:
		// defers function argument to the end of the current frame.
		// this is safe to call from any thread
		template<typename TFunctor, typename ...TArgs>
		void defer(TFunctor&& f, TArgs&&... args)
		{
			std::packaged_task<void()> task(
				[f = std::forward<TFunctor>(f), ...args = std::forward<TArgs>(args)]() mutable
				{
					std::invoke(std::move(f), std::move(args)...);
				}
			);

			{
				std::scoped_lock<std::mutex> lock(m_mutex);
				m_queue.enqueue([t = std::move(task)]() mutable { t();  });
			}
		}

		// resolves all the defered calls.
		void resolve()
		{
			SizeT count = 0;
			{
				std::scoped_lock<std::mutex> lock(m_mutex);
				count = m_queue.count();
			}

			// make sure we resolve only what was deferred this frame.
			for (SizeT i = 0; i < count; i++)
			{
				std::move_only_function<void()> f;
				{
					std::scoped_lock<std::mutex> lock(m_mutex);
					f = std::move(m_queue.dequeue());
				}
				f();
			}
		}

	private:
		std::mutex m_mutex;
		Mani::List<std::move_only_function<void()>> m_queue;
	};
}