#pragma once

#include <Core/CoreFwd.h>
#include <fmod.hpp>
#include <string>

namespace Mani
{
	constexpr std::string_view LogFMod = "FMod";

	struct FMod
	{
		FMOD::System* system = nullptr;
	};

	namespace FModStatics
	{
		FMod& getFModChecked(ECS::Registry& registry);
		ECS::EntityId play(ECS::Registry& registry, ECS::EntityId resourceId);
		ECS::EntityId loop(ECS::Registry& registry, ECS::EntityId resourceId);
		
		void pause(ECS::Registry& registry, ECS::EntityId entityId);
		void resume(ECS::Registry& registry, ECS::EntityId entityId);

		void setVolume(ECS::Registry& registry, ECS::EntityId entityId, float volume);

		void stop(ECS::Registry& registry, ECS::EntityId entityId);
	}
}