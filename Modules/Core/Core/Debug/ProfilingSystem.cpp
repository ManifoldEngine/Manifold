#include "ProfilingSystem.h"
#include <Core/Debug/Profiling.h>

#include <unordered_map>
#include <ManiMaths/Fwd.h>

using namespace Mani;

struct ScopedTimerStats
{
	unsigned long count = 0;
	unsigned long accumulator = 0;
	unsigned long min = 0;
	unsigned long max = 0;
};

std::unordered_map<std::string, ScopedTimerStats> STATS;


void ProfilingSystem::onInitialize(Mani::ECS::Registry& registry, Mani::SystemContainer& systemContainer)
{
	STATS.clear();
}

void ProfilingSystem::onDeinitialize(Mani::ECS::Registry& registry)
{
	for (const auto& [name, stat] : STATS)
	{
		MANI_LOG(getName(), "Scoped event: {}", name);
		MANI_LOG(getName(), "|--count: {}", stat.count);
		MANI_LOG(getName(), "|--average: {}ms", stat.accumulator / stat.count);
		MANI_LOG(getName(), "|--min: {}ms", stat.min);
		MANI_LOG(getName(), "|--max: {}ms", stat.max);
	}
	STATS.clear();
}

void ProfilingSystem::onTimerDestroyed(const Mani::_impl::ScopedTimer& scopeTimer)
{
	const unsigned long elapsed = scopeTimer.elapsed();

	ScopedTimerStats& stats = STATS[scopeTimer.name];
	stats.count++;
	stats.accumulator += elapsed;
	stats.min = Mani::Math::minT(stats.min, elapsed);
	stats.max = Mani::Math::maxT(stats.max, elapsed);
}