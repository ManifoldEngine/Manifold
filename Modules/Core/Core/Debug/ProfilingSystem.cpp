#include "ProfilingSystem.h"
#include <Core/Debug/Profiling.h>

#include <ManiMaths/Fwd.h>

using namespace Mani;

void ProfilingSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<ScopedTimerDatabase>();
}

void ProfilingSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	for (const auto entityId : ECS::View<ScopedTimerDatabase>(registry))
	{
		const ScopedTimerDatabase& database = *registry.get<ScopedTimerDatabase>(entityId);
		for (const auto& [name, stat] : database.scopedTimers)
		{
			MANI_LOG(getName(), "Scoped event: {}", name);
			MANI_LOG(getName(), "|----count: {}", stat.count);
			MANI_LOG(getName(), "|--average: {:.3}ms", stat.accumulator / stat.count);
			MANI_LOG(getName(), "|------min: {:.3}ms", stat.min);
			MANI_LOG(getName(), "|------max: {:.3}ms", stat.max);
		}
	}
	registry.removeSingle<ScopedTimerDatabase>();
}

void ProfilingSystem::onTimerDestroyed(const _impl::ScopedTimer& scopeTimer)
{
#if MANI_DEBUG
	ECS::Registry& registry = Application::get().getWorld().getMutableRegistry();
	if (ScopedTimerDatabase* database = registry.getSingle<ScopedTimerDatabase>())
	{
		const double elapsed = scopeTimer.elapsed();

		ScopedTimerStats& stats = database->scopedTimers[scopeTimer.name];
		stats.count++;
		stats.accumulator += elapsed;
		stats.min = Math::minT(stats.min, elapsed);
		stats.max = Math::maxT(stats.max, elapsed);
		stats.lastValue = elapsed;

		MANI_LOG_VERBOSE("ProfilingSystem", "{}: {}ms", scopeTimer.name, elapsed);
	}
#endif
}