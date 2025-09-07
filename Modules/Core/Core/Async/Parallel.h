#pragma once

#include <Core/Application.h>
#include <Core/ECS/Registry.h>
#include <Core/ECS/View.h>

#include <ManiMaths/Maths.h>
#include <latch>
#include <type_traits>

namespace Mani
{
	template<typename... TComponents, typename TFunctor = void(const ECS::EntityId, size_t)>
	void parallelFor(ECS::View<TComponents...>& view, TFunctor&& f)
	{
		static_assert(Application::THREAD_COUNT > 0, "cannot parallel over 0 threads");

		const size_t unadjustedSize = view.unadjustedSize();
		size_t chunkSize = unadjustedSize / Application::THREAD_COUNT;
		if (unadjustedSize % Application::THREAD_COUNT > 0)
		{
			chunkSize += 1;
		}

		ThreadPool& threadPool = Application::get().getThreadPool();
		std::latch latch{ Application::THREAD_COUNT };
		for (size_t threadIndex = 0; threadIndex < Application::THREAD_COUNT; threadIndex++)
		{
			ECS::EntityId start = threadIndex * chunkSize;
			ECS::EntityId end = start + chunkSize;

			threadPool.enqueue([&latch, &view, threadIndex, start, end, &f] 
			{
				const auto viewEnd = view.end();
				for (auto it = view.at(start); *it < end && it != viewEnd; ++it)
				{
					f(*it, threadIndex);
				}
				latch.count_down();
			});
		}

		latch.wait();
	}

	template<typename TFunctor, typename... TArgs>
	auto enqueueTask(TFunctor&& f, TArgs&&... args)
	{
		ThreadPool& threadPool = Application::get().getThreadPool();
		return threadPool.enqueue(std::forward<TFunctor>(f), std::forward<TArgs>(args)...);
	}

	template<typename TFunctor, typename... TArgs>
	void defer(TFunctor&& f, TArgs&&... args)
	{
		Deferred& deferred = Application::get().getDeferred();
		deferred.defer(std::forward<TFunctor>(f), std::forward<TArgs>(args)...);
	}
}