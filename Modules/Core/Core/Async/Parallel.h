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

		const ECS::Index count = view.count();
		if (count == 0)
		{
			return;
		}

		ECS::Index chunkSize = static_cast<ECS::Index>(count / Application::THREAD_COUNT);
		if (count % Application::THREAD_COUNT > 0)
		{
			chunkSize += 1;
		}

		ThreadPool& threadPool = Application::get().getThreadPool();
		std::latch latch{ Application::THREAD_COUNT };
		for (ECS::Index threadIndex = 0; threadIndex < Application::THREAD_COUNT; threadIndex++)
		{
			ECS::Index start = threadIndex * chunkSize;
			ECS::Index end = start + chunkSize;

			threadPool.enqueue([&latch, &view, threadIndex, start, end, &f] 
			{
				auto it = view.at(start);
				const auto viewEnd = view.end();
				if (it.getIndex() < end && it < viewEnd)
				{
					// TODO DIRTY FIX: The view count isn't representing the amount of entity in the view, but rather the total amount 
					// of entities in the registry.
					// This means that the amount of threads dedicated to this view might be disproportionate to the amount of work
					//
					// additionally it is possible that the first index is not actually part of the view and needs to be checked before
					// being sent to the functor. This would be fixed by sparse component pools because the view count would be accurate
					// and we would only iterate over valid memory.
					const ECS::Registry& registry = view.getRegistry();
					if (!registry.isValid(*it) || !registry.has<TComponents...>(*it))
					{
						// if the first viewed entity isn't valid for this view, go to the next one.
						++it;
					}

					for (; it.getIndex() < end && it < viewEnd; ++it)
					{	
						f(*it, threadIndex);
					}
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