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
			ScopedTimer(const std::string_view& _name, const char* file, unsigned int line) : name(_name), id(RecordId{ file, line })
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
						.id = id,
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

			RecordId id = INVALID_RECORD_ID;
			std::string_view name;
			std::chrono::steady_clock::time_point start;
			std::chrono::steady_clock::time_point end;
		};
	}
}

#ifdef MANI_PROFILING
#define MANI_TIME_SCOPE(NAME) \
	Mani::Profiling::ScopedTimer scopeTimer_##__LINE__(NAME, __FILE__, __LINE__)
#else
#define MANI_TIME_SCOPE(NAME)
#endif