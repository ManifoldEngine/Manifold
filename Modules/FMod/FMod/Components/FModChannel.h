#pragma once

#include <fmod.h>

namespace Mani
{
	struct FModChannel
	{
		ECS::EntityId resourceId = ECS::INVALID_ID;
		FMOD::Channel* value = nullptr;
	
		bool isPaused = false;
		float volume = 1.f;
	};
}