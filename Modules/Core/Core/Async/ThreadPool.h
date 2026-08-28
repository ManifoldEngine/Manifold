#pragma once

#include <Core/Core.h>
#include <Core/Containers/List.h>

#include <mutex>
#include <thread>
#include <future>
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

			m_threads.reserve(size);
			m_stopRequested = false;
			for (size_t i = 0; i < size; ++i)
			{
				m_threads.add(std::thread([this]() { worker(); }));
			}
		}

		void stop()
		{
			{
				std::scoped_lock<std::mutex> lock(m_mutex);
				m_stopRequested = true;
			}

			m_condition_variable.notify_all();

			for (std::thread& thread : m_threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
			m_threads.clear();
		}

		bool isRunning() const { return !m_threads.isEmpty(); }

		template<typename TFunctor, typename... TArgs>
		void enqueue(TFunctor&& f, TArgs&&... args)
		{
			std::packaged_task<void()> task(
				[f = std::forward<TFunctor>(f), ...args = std::forward<TArgs>(args)]() mutable
				{
					std::invoke(std::move(f), std::move(args)...);
				}
			);
			{
				std::scoped_lock<std::mutex> lock(m_mutex);
				MANI_ASSERT(!m_stopRequested, "enqueue on stopped Worker");
				m_queue.enqueue([t = std::move(task)]() mutable { t(); });
			}
			m_condition_variable.notify_one();
		}
	private:
		void worker()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			while (!m_stopRequested || !m_queue.isEmpty())
			{
				m_condition_variable.wait(lock, [this]() {
					return m_stopRequested || !m_queue.isEmpty();
				});

				if (m_stopRequested && m_queue.isEmpty())
				{
					break;
				}

				auto task = std::move(m_queue.dequeue());
				lock.unlock();
				task();
				lock.lock();
			}
		};

		bool m_stopRequested = false;

		mutable std::mutex m_mutex;
		std::condition_variable m_condition_variable;
		List<std::thread> m_threads;
		List<std::move_only_function<void()>> m_queue;
	};
}