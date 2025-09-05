#include "TimeSystem.h"
#include <Core/ManiTime.h>
#include <chrono>
#include <format>

using namespace Mani;
using ManiClock = std::chrono::steady_clock;

struct TimeSystem::Storage
{
	ManiClock::time_point previousTimePoint;
};

void TimeSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<Time>();
	Storage& storage = *registry.addSingle<Storage>();
	storage.previousTimePoint = ManiClock::now();
}

void TimeSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	registry.removeSingle<Storage>();
	registry.removeSingle<Time>();
}

void TimeSystem::tick(ECS::Registry& registry)
{
	Time& time = *registry.getSingle<Time>();
	Storage& storage = *registry.getSingle<Storage>();
	ManiClock::time_point now = ManiClock::now();
	ManiClock::duration delta = now - storage.previousTimePoint;

	time.realDelta = std::chrono::seconds::duration<float>(delta).count();
	time.delta = time.realDelta * time.scale;

	time.realElapsed += time.realDelta;
	time.elapsed += time.delta;

	storage.previousTimePoint = now;
}

std::string TimeSystem::getTimeFormatted()
{
#if MANI_WEBGL
	// emscripten does not support timezones from std::chrono
	const auto now = std::chrono::system_clock::now();
	return std::format("{:%T}", std::chrono::floor<std::chrono::microseconds>(now));
#else
	auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	return std::format("{:%T}", now);
#endif
}