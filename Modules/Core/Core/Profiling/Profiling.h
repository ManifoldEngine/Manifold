#pragma once

#include <Core/Application.h>
#include <Core/Containers/Array.h>
#include <Core/Containers/List.h>
#include <Core/Profiling/Profiler.h>
#include <string>
#include <chrono>

namespace Mani
{
	namespace Profiling
	{
		inline ThreadProfiler* getThreadProfiler()
		{
			thread_local ThreadProfiler* profiler = nullptr;
			if (profiler == nullptr && Application::exists())
			{
				profiler = Application::get().getProfiler().reserveProfiler();
			}
			return profiler;
		}

		struct ScopedTimer
		{
			ScopedTimer(const std::string_view& nameIn) : name(nameIn)
			{
				start = std::chrono::steady_clock::now();
				end = start;
			}

			~ScopedTimer()
			{
				end = std::chrono::steady_clock::now();
				if (ThreadProfiler* profiler = getThreadProfiler())
				{
					profiler->record({
						.name = name,
						.duration = elapsed(),
					});
				}
			}

			double elapsed() const
			{
				constexpr double secondsToMs = 1000.0;
				return secondsToMs * std::chrono::duration<double>(end - start).count();
			};

			std::string_view name;
			std::chrono::steady_clock::time_point start;
			std::chrono::steady_clock::time_point end;
		};
	}
}

#ifdef MANI_PROFILING
#define MANI_TIME_SCOPE(NAME) \
	Mani::Profiling::ScopedTimer scopeTimer_##__LINE__(NAME)
#else
#define MANI_TIME_SCOPE(NAME)
#endif