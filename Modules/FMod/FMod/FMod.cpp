#include "FMod.h"

#include <Resources/Components/Resource.h>

#include <FMod/Resources/FModSound.h>
#include <FMod/Resources/FModStream.h>

#include <FMod/Components/FModChannel.h>
#include <FMod/Components/FModPlayQueue.h>

using namespace Mani;

Ref<FMod> FModControls::getFModChecked(ECS::Registry& registry)
{
	Ref<FMod> fmod = registry.getSingle<FMod>();
	MANI_ASSERT(fmod.isValid(), "FModSystem isn't initialized.");
	return fmod;
}

void FModControls::play(ECS::Registry& registry, ECS::EntityId channelId)
{
	Ref<FModPlayQueue> queue = registry.getSingle<FModPlayQueue>();
	queue->value.enqueue(channelId);
}

void FModControls::pause(ECS::Registry& registry, ECS::EntityId channelId)
{
	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->isPaused = true;
	if (channel->value != nullptr)
	{
		channel->value->setPaused(channel->isPaused);
	}
}

void FModControls::resume(ECS::Registry& registry, ECS::EntityId channelId)
{
	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->isPaused = false;
	if (channel->value != nullptr)
	{
		channel->value->setPaused(channel->isPaused);
	}
}

void FModControls::setVolume(ECS::Registry& registry, ECS::EntityId channelId, float volume)
{
	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->volume = volume;
	if (channel->value != nullptr)
	{
		channel->value->setVolume(channel->volume);
	}
}
