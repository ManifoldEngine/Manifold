#include "CoreTime.h"
#include <chrono>

using namespace ECSEngine;

static float sm_deltaTime;

using time_point = std::chrono::high_resolution_clock::time_point;
static time_point sm_applicationStartTime;
static time_point sm_lastestFrameTime;

auto getNow()
{
	return std::chrono::high_resolution_clock::now();
}

float Time::getTime()
{
	auto now = getNow();
	std::chrono::duration<float> delta = now - sm_applicationStartTime;
	return delta.count();
}

float Time::getDeltaTime()
{
	return sm_deltaTime;
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
