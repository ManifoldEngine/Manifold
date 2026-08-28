#pragma once

#include <Core/Application.h>
#include <Core/ECS/Registry.h>
#include <Core/ECS/View.h>
#include <Core/ECS/CommandBuffer.h>

#include <ManiMaths/Maths.h>
#include <tuple>
#include <latch>
#include <type_traits>

namespace Mani
{	
	constexpr SizeT DEFAULT_MIN_CHUNK_SIZE = 100;

	template<typename... Ts, typename TFunctor>
	void parallelForWithCmd(ECS::View<Ts...>& view, TFunctor&& f, SizeT minChunkSize = DEFAULT_MIN_CHUNK_SIZE)
	{
		// go wide
		MANI_ASSERT(minChunkSize > 0, "Cannot cut up the work into chunks of 0");
		static_assert(Application::THREAD_COUNT > 0, "cannot parallel over 0 threads");
		SizeT count = view.count();
		if (count == 0)
		{
			return;
		}

		const SizeT workPerThread = count / Application::THREAD_COUNT;
		SizeT chunkSize = workPerThread;
		if (count % Application::THREAD_COUNT > 0)
		{
			chunkSize += 1;
		}

		if (workPerThread == 0 || chunkSize < minChunkSize)
		{
			// too wide
			if constexpr (std::is_invocable_v<TFunctor, ECS::CommandBuffer&, SizeT, ECS::EntityId, Ts&...>)
			{
				Mani::foreachWithCmd(view, std::forward<TFunctor>(f), 0);
			}
			else if constexpr (std::is_invocable_v<TFunctor, ECS::CommandBuffer&, ECS::EntityId, Ts&...>)
			{
				Mani::foreachWithCmd(view, std::forward<TFunctor>(f));
			}
			else
			{
				static_assert(false, "Bad lambda");
			}
			return;
		}

		Mani::List<ECS::CommandBuffer> cmdBuffers(Application::THREAD_COUNT, ECS::CommandBuffer(view.getRegistry()));

		ThreadPool& threadPool = Application::get().getThreadPool();
		std::latch latch{ static_cast<ptrdiff_t>(Application::THREAD_COUNT) };

		for (SizeT threadIdx = 0; threadIdx < Application::THREAD_COUNT; threadIdx++)
		{
			const SizeT startIndex = threadIdx * chunkSize;
			const SizeT endIndex = Math::minT(startIndex + chunkSize, count);
			ECS::CommandBuffer& cmd = cmdBuffers[threadIdx];
			threadPool.enqueue([&latch, &view, &cmd, threadIdx, startIndex, endIndex, &f] mutable
			{
				auto base = view.begin();
				auto it = base + startIndex;
				const auto end = base + endIndex;
				for (; it != end; ++it)
				{
					if constexpr (std::is_invocable_v<TFunctor, ECS::CommandBuffer&, SizeT, ECS::EntityId, Ts&...>)
					{
						it.apply(f, cmd, threadIdx);
					}
					else if constexpr (std::is_invocable_v<TFunctor, ECS::CommandBuffer&, ECS::EntityId, Ts&...>)
					{
						it.apply(f, cmd);
					}
					else
					{
						static_assert(false, "Bad lambda");
					}
				}
				latch.count_down();
			});
		}

		latch.wait();

		view.unlock();
		for (auto& cmd : cmdBuffers)
		{
			cmd.execute();
		}
		view.lock();
	}

	template<typename... Ts, typename TFunctor, typename TRegistry, typename TArchetype>
	void parallelFor(ECS::BaseView<TRegistry, TArchetype, Ts...>& view, TFunctor&& f, SizeT minChunkSize = DEFAULT_MIN_CHUNK_SIZE)
	{
		// go wide
		MANI_ASSERT(minChunkSize > 0, "Cannot cut up the work into chunks of 0");
		static_assert(Application::THREAD_COUNT > 0, "cannot go wide over 0 threads");
		SizeT count = view.count();
		if (count == 0)
		{
			return;
		}

		const SizeT workPerThread = count / Application::THREAD_COUNT;
		SizeT chunkSize = workPerThread;
		if (count % Application::THREAD_COUNT > 0)
		{
			chunkSize += 1;
		}

		if (workPerThread == 0 || chunkSize < minChunkSize)
		{
			// too wide
			if constexpr (std::is_invocable_v<TFunctor, SizeT, ECS::EntityId, Ts&...>)
			{
				Mani::foreach(view, std::forward<TFunctor>(f), 0);
			}
			else if constexpr (std::is_invocable_v<TFunctor, ECS::EntityId, Ts&...>)
			{
				Mani::foreach(view, std::forward<TFunctor>(f));
			}
			else
			{
				static_assert(false, "Bad lambda");
			}
			return;
		}
		
		ThreadPool& threadPool = Application::get().getThreadPool();
		std::latch latch{ static_cast<ptrdiff_t>(Application::THREAD_COUNT) };

		for (SizeT threadIdx = 0; threadIdx < Application::THREAD_COUNT; threadIdx++)
		{
			const SizeT startIndex = threadIdx * chunkSize;
			const SizeT endIndex = Math::minT(startIndex + chunkSize, count);
			threadPool.enqueue([&latch, &view, threadIdx, startIndex, endIndex, &f] mutable
			{
				auto base = view.begin();
				auto it = base + startIndex;
				const auto end = base + endIndex;
				for (; it < end; ++it)
				{
					if constexpr (std::is_invocable_v<TFunctor, SizeT, ECS::EntityId, Ts&...>)
					{
						it.apply(f, threadIdx);
					}
					else if constexpr (std::is_invocable_v<TFunctor, ECS::EntityId, Ts&...>)
					{
						it.apply(f);
					}
					else
					{
						static_assert(false, "Bad lambda");
					}
				}
				latch.count_down();
			});
		}

		latch.wait();
	}

	template<typename TFunctor, typename... TArgs>
	void enqueueTask(TFunctor&& f, TArgs&&... args)
	{
		ThreadPool& threadPool = Application::get().getThreadPool();
		threadPool.enqueue(std::forward<TFunctor>(f), std::forward<TArgs>(args)...);
	}

	template<typename TFunctor, typename... TArgs>
	void defer(TFunctor&& f, TArgs&&... args)
	{
		Deferred& deferred = Application::get().getDeferred();
		deferred.defer(std::forward<TFunctor>(f), std::forward<TArgs>(args)...);
	}
}