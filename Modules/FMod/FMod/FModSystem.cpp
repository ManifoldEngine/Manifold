#include "FModSystem.h"

#include <Core/Profiling/Profiling.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModResourceSystem.h>
#include <FMod/FModPlaySystem.h>

#include <fmod.hpp>
#include <fmod_errors.h>

using namespace Mani;

void FModSystem::onInitialize(ECS::Registry& registry, World& world)
{
	Ref<FMod> fmod = registry.addSingle<FMod>();
	FMOD_RESULT result = FMOD::System_Create(&fmod->system);
	if (result != FMOD_OK)
	{
		MANI_LOG_ERROR(LogFMod, "Failed to create FMod System: {}", FMOD_ErrorString(result));
		return;
	}

	constexpr int maxChannels = 512;
	result = fmod->system->init(maxChannels, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
	{
		MANI_LOG_ERROR(LogFMod, "Failed to initializze FMod System: {}", FMOD_ErrorString(result));
		return;
	}

	world.createSystem<FModResourceSystem>()
		 .createSystem<FModPlaySystem>();
}

void FModSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Ref<FMod> fmod = registry.getSingle<FMod>();
	if (fmod->system != nullptr)
	{
		fmod->system->close();
		fmod->system->release();
		fmod->system = nullptr;
	}

	world.destroySystem<FModResourceSystem>()
		 .destroySystem<FModPlaySystem>();

	registry.removeSingle<FMod>();
}

void FModSystem::tick(ECS::Registry& registry)
{
	Ref<FMod> fmod = registry.getSingle<FMod>();
	if (fmod->system != nullptr)
	{
		fmod->system->update();
	}
}
