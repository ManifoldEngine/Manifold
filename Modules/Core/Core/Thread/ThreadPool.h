#pragma once

#include <Core/Core.h>

#include <mutex>
#include <vector>
#include <thread>
#include <future>
#include <queue>
#include <functional>
#include <memory>

namespace Mani
{
	class ThreadPool
	{
	public:
		ThreadPool() = default;

		ThreadPool(size_t size)
		{
			start(size);
		}

		~ThreadPool()
		{
			stop();
		}

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		void start(size_t size)
		{
			MANI_ASSERT(!isRunning(), "Trying to start a thread pool that is already running.");

			m_threads = std::vector<std::thread>(size);
			m_busyThreads = size;
			m_stopRequested = false;
			for (size_t i = 0; i < size; ++i)
			{
				m_threads.emplace_back([this]() { worker(); });
			}
		}

		void stop()
		{
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_stopRequested = true;
				m_condition_variable.notify_all();
			}

			for (std::thread& thread : m_threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
		}

		bool isRunning() const { return !m_threads.empty(); }

		template<typename TFunctor, typename... TArgs>
		auto enqueue(TFunctor&& f, TArgs&&... args)
		{
			using TReturn = decltype(f(args...));

			auto taskPtr = std::make_shared<std::packaged_task<TReturn()>>(
				std::bind(std::forward<TFunctor>(f), std::forward<TArgs>(args)...)
			);

			std::future<TReturn> future = taskPtr->get_future();
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_queue.emplace([taskPtr]() { (*taskPtr)(); });
			}
			m_condition_variable.notify_one();
			return future;
		}
	private:
		void worker()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			while (!m_stopRequested || (m_stopRequested && !m_queue.empty()))
			{
				m_busyThreads--;
				m_condition_variable.wait(lock, [this]() {
					return m_stopRequested || !m_queue.empty();
				});
				m_busyThreads++;

				if (!m_queue.empty())
				{
					auto task = std::move(m_queue.front());
					m_queue.pop();
					lock.unlock();
					task();
					lock.lock();
				}
			}
		};

		size_t m_size = 0;
		size_t m_busyThreads = 0;
		bool m_stopRequested = false;

		mutable std::mutex m_mutex;
		std::condition_variable m_condition_variable;
		std::vector<std::thread> m_threads;
		std::queue<std::function<void()>> m_queue;
	};
}