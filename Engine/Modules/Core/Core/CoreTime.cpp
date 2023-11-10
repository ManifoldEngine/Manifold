#include "CoreTime.h"
#include <chrono>
#include <format>

using namespace ECSEngine;

static float sm_deltaTime;

using time_point = std::chrono::high_resolution_clock::time_point;
static time_point sm_applicationStartTime;
static time_point sm_lastestFrameTime;

auto getNow()
{
	return std::chrono::high_resolution_clock::now();
}

float Time::getTimeSinceStart()
{
	auto now = getNow();
	std::chrono::duration<float> delta = now - sm_applicationStartTime;
	return delta.count();
}

float Time::getDeltaTime()
{
	return sm_deltaTime;
}

std::string Time::getTimeFormatted()
{
	auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	return std::format("{:%T}", now);;
}

void Time::onApplicationStart()
{
	sm_applicationStartTime = getNow();
	sm_lastestFrameTime = getNow();
}

void Time::onNewFrame()
{
	auto now = getNow();
	std::chrono::duration<float> delta = now - sm_lastestFrameTime;
	sm_deltaTime = delta.count();
	sm_lastestFrameTime = now;
}
