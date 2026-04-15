#pragma once

#include <Core/CoreFwd.h>
#include <fmod.hpp>
#include <string>

namespace Mani
{
	constexpr LogChannel LogFMod("FMod");

	struct FMod
	{
		FMOD::System* system = nullptr;
	};

	namespace FModStatics
	{
		Ref<FMod> getFModChecked(ECS::Registry& registry);
		ECS::EntityId play(ECS::Registry& registry, ECS::EntityId resourceId);
		ECS::EntityId loop(ECS::Registry& registry, ECS::EntityId resourceId);
		
		void pause(ECS::Registry& registry, ECS::EntityId channelId);
		void resume(ECS::Registry& registry, ECS::EntityId channelId);

		void setVolume(ECS::Registry& registry, ECS::EntityId channelId, float volume);

		void stop(ECS::Registry& registry, ECS::EntityId channelId);
	}
}