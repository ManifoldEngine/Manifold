#pragma once

#include <Core/Containers/Map.h>
#include <Core/Debug/ProfilingSystem.h>
#include <string>
#include <chrono>

namespace Mani
{
	namespace _impl
	{
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
				Mani::ProfilingSystem::onTimerDestroyed(*this);
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

	struct ScopedTimerStats
	{
		unsigned long count = 0;
		double accumulator = 0.0;
		double min = 0.0;
		double max = 0.0;
		double lastValue = 0.0;

		double perTickAccumulator = 0.0;
		double lastTick = 0.0;
	};

	struct ScopedTimerDatabase
	{
		Map<std::string, ScopedTimerStats> scopedTimers;
	};
}

//#if MANI_DEBUG
//#define MANI_TIME_SCOPE(NAME) \
//	Mani::_impl::ScopedTimer scopeTimer_##__LINE__(NAME)
//#else
#define MANI_TIME_SCOPE(NAME)
//#endif