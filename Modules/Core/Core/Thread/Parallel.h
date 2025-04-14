#pragma once

#include <Core/Application.h>

#include <ECS/Registry.h>
#include <ECS/View.h>

#include <ManiMaths/Maths.h>
#include <latch>

namespace Mani
{
	template<typename... TComponents, typename TFunctor>
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

			threadPool.enqueue([&latch, &view, start, end, &f] {
				const auto viewEnd = view.end();
				for (auto it = view.at(start); *it < end && it != viewEnd; ++it)
				{
					f(view.getRegistry(), *it);
				}
				latch.count_down();
			});
		}

		latch.wait();
	}
}