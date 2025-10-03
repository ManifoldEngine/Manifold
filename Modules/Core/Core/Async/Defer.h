#pragma once

#include <Core/Containers/List.h>
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
			auto taskPtr = std::make_shared<std::function<void()>>(
				std::bind(std::forward<TFunctor>(f), std::forward<TArgs>(args)...)
			);

			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_queue.enqueue([taskPtr]() { (*taskPtr)(); });
			}
		}

		// resolves all the defered calls.
		void resolve()
		{
			while (!m_queue.isEmpty())
			{
				auto f = std::move(m_queue.first());
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					m_queue.dequeue();
				}
				f();
			}
		}

	private:
		std::mutex m_mutex;
		Mani::List<std::function<void()>> m_queue;
	};
}