#include "ProfilingSystem.h"
#include <Core/Debug/Profiling.h>

#include <ManiMaths/Fwd.h>
#include <mutex>

using namespace Mani;

struct ProfilingSystem::Storage
{
	struct NamedScopedTimer
	{
		std::string name = "";
		ScopedTimerStats timer;
	};

	std::mutex mutex;
	List<NamedScopedTimer> frame;
};

void ProfilingSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<ScopedTimerDatabase>();
	registry.addSingle<Storage>();
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
	registry.removeSingle<Storage>();
}

void ProfilingSystem::tick(ECS::Registry& registry)
{
	ScopedTimerDatabase& database = *registry.getSingle<ScopedTimerDatabase>();
	Storage& storage = *registry.getSingle<Storage>();

	{
		// consume frame
		std::scoped_lock<std::mutex> lock(storage.mutex);

		for (const auto& [name, delta] : storage.frame)
		{
			ScopedTimerStats& stats = database.scopedTimers[name];
			stats.count += delta.count;
			stats.accumulator += delta.accumulator;
			stats.perTickAccumulator += delta.perTickAccumulator;
			stats.min = Math::minT(stats.min, delta.min);
			stats.max = Math::maxT(stats.max, delta.max);
			stats.lastValue = delta.lastValue;
		}

		storage.frame.clear();
	}

	for (auto& [name, timer] : database.scopedTimers)
	{
		timer.lastTick = timer.perTickAccumulator;
		timer.perTickAccumulator = 0.f;
	}
}

void ProfilingSystem::onTimerDestroyed(const _impl::ScopedTimer& scopeTimer)
{
#if MANI_DEBUG
	if (!Application::exists())
	{
		return;
	}

	ECS::Registry& registry = Application::get().getWorld().getMutableRegistry();
	if (Storage* storage = registry.getSingle<Storage>())
	{
		// this mighe get called from any thread
		std::scoped_lock<std::mutex> lock(storage->mutex);
		const double elapsed = scopeTimer.elapsed();

		// create a stats instance that contains the delta for this scoped timer.
		ScopedTimerStats stats;
		stats.count = 1;
		stats.accumulator = elapsed;
		stats.perTickAccumulator = elapsed;
		stats.min = elapsed;
		stats.max = elapsed;
		stats.lastValue = elapsed;
		
		storage->frame.add({ scopeTimer.name, stats });
		MANI_LOG_VERBOSE("ProfilingSystem", "{}: {}ms", scopeTimer.name, elapsed);
	}
#endif
}