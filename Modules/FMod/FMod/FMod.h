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

	// To play a sound:
	//	- first add a FModChannel component with the resource's you want to play to an entity
	//	- add any other components relevant to the play type (for example FModOneShot)
	//	- then call the play functions with that channel's entity id
	//	- destroy the channels along with the owning entity or if the resource shall not be played anymore
	namespace FModControls
	{
		Ref<FMod> getFModChecked(ECS::Registry& registry);
		void play(ECS::Registry& registry, ECS::EntityId channelId);
		
		void pause(ECS::Registry& registry, ECS::EntityId channelId);
		void resume(ECS::Registry& registry, ECS::EntityId channelId);

		void setVolume(ECS::Registry& registry, ECS::EntityId channelId, float volume);
	}
}