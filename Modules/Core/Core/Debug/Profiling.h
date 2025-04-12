#pragma once

#include <Core/CoreFwd.h>
#include <Core/Debug/ProfilingSystem.h>
#include <string>
#include <ctime>

namespace Mani
{
	namespace _impl
	{
		struct ScopedTimer
		{
			ScopedTimer(std::string nameIn) : name(nameIn)
			{
				start = std::clock();
				end = start;
			}

			~ScopedTimer() 
			{
				end = std::clock();
				Mani::ProfilingSystem::onTimerDestroyed(*this);
			}

			double elapsed() const 
			{
				constexpr double secondsToMs = 1000.0;
				return secondsToMs * (static_cast<double>(end - start) / static_cast<double>(CLOCKS_PER_SEC));
			};

			std::string name;
			std::clock_t start;
			std::clock_t end;
		};
	}

	struct ScopedTimerStats
	{
		unsigned long count = 0;
		double accumulator = 0;
		double min = 0;
		double max = 0;
		double lastValue = 0;
	};

	struct ScopedTimerDatabase
	{
		std::unordered_map<std::string, ScopedTimerStats> scopedTimers;
	};
}

#if MANI_DEBUG
#define MANI_TIME_SCOPE(NAME) Mani::_impl::ScopedTimer scopeTimer##NAME(#NAME)
#else
#define MANI_TIME_SCOPE(NAME)
#endif