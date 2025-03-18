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
				MANI_LOG_VERBOSE("Timer", "{}: {}ms", name, elapsed());
			}

			std::clock_t elapsed() const { return (end - start) / CLOCKS_PER_SEC; };

			std::string name;
			std::clock_t start;
			std::clock_t end;
		};
	}
}

#if MANI_DEBUG
#define MANI_TIME_SCOPE(NAME) Mani::_impl::ScopedTimer scopeTimer##NAME(#NAME)
#else
#define MANI_TIME_SCOPE(NAME)
#endif