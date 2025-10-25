#include "FModSystem.h"

#include <Core/Debug/Profiling.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModResourceSystem.h>
#include <FMod/FModPlaySystem.h>
#include <FMod/FModOneShotSystem.h>

#include <fmod.hpp>
#include <fmod_errors.h>

using namespace Mani;

void FModSystem::onInitialize(ECS::Registry& registry, World& world)
{
	FMod& fmod = *registry.addSingle<FMod>();
	FMOD_RESULT result = FMOD::System_Create(&fmod.system);
	if (result != FMOD_OK)
	{
		MANI_LOG_ERROR(LogFMod, "Failed to create FMod System: {}", FMOD_ErrorString(result));
		return;
	}

	constexpr int maxChannels = 512;
	result = fmod.system->init(maxChannels, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
	{
		MANI_LOG_ERROR(LogFMod, "Failed to initializze FMod System: {}", FMOD_ErrorString(result));
		return;
	}

	world.createSystem<FModResourceSystem>()
		 .createSystem<FModPlaySystem>()
		 .createSystem<FModOneShotSystem>();
}

void FModSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	FMod& fmod = *registry.getSingle<FMod>();
	if (fmod.system != nullptr)
	{
		fmod.system->close();
		fmod.system->release();
		fmod.system = nullptr;
	}

	world.destroySystem<FModResourceSystem>()
		 .destroySystem<FModPlaySystem>()
		 .destroySystem<FModOneShotSystem>();

	registry.removeSingle<FMod>();
}

void FModSystem::tick(ECS::Registry& registry)
{
	FMod& fmod = *registry.getSingle<FMod>();
	if (fmod.system != nullptr)
	{
		MANI_TIME_SCOPE(FModSystem_tick_update);
		fmod.system->update();
	}
}
