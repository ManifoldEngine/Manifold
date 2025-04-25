#pragma once

#include <queue>
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
				m_queue.emplace([taskPtr]() { (*taskPtr)(); });
			}
		}

		// resolves all the defered calls.
		void resolve()
		{
			while (!m_queue.empty())
			{
				auto f = std::move(m_queue.front());
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					m_queue.pop();
				}
				f();
			}
		}

	private:
		std::mutex m_mutex;
		std::queue<std::function<void()>> m_queue;
	};
}