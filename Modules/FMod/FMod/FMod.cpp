#include "FMod.h"

#include <Resources/Components/Resource.h>

#include <FMod/Resources/FModSound.h>
#include <FMod/Resources/FModStream.h>

#include <FMod/Components/FModChannel.h>
#include <FMod/Components/FModPlayQueue.h>
#include <FMod/Components/FModOneShot.h>

using namespace Mani;

ECS::EntityId createChannel(ECS::Registry& registry)
{
	ECS::EntityId entityId = registry.create();
	registry.add<FModChannel>(entityId);
	return entityId;
}

Ref<FMod> FModStatics::getFModChecked(ECS::Registry& registry)
{
	Ref<FMod> fmod = registry.getSingle<FMod>();
	MANI_ASSERT(fmod.isValid(), "FModSystem isn't initialized.");
	return fmod;
}

ECS::EntityId FModStatics::play(ECS::Registry& registry, ECS::EntityId resourceId)
{
	MANI_ASSERT(registry.has<Resource<FModSound>>(resourceId), "trying to play something that is not a FModSound");
	Ref<FModPlayQueue> queue = registry.getSingle<FModPlayQueue>();
	ECS::EntityId channelId = createChannel(registry);

	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->resourceId = resourceId;

	registry.add<FModOneShot>(channelId);
	
	queue->value.enqueue(channelId);
	return channelId;
}

ECS::EntityId FModStatics::loop(ECS::Registry& registry, ECS::EntityId resourceId)
{
	MANI_ASSERT(registry.has<Resource<FModSound>>(resourceId), "trying to play something that is not a FModSound");
	Ref<FModPlayQueue> queue = registry.getSingle<FModPlayQueue>();
	ECS::EntityId channelId = createChannel(registry);

	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->resourceId = resourceId;
	
	queue->value.enqueue(channelId);
	return channelId;
}

void FModStatics::pause(ECS::Registry& registry, ECS::EntityId channelId)
{
	Ref<FMod> fmod = FModStatics::getFModChecked(registry);
	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->isPaused = true;
	if (channel->value != nullptr)
	{
		channel->value->setPaused(channel->isPaused);
	}
}

void FModStatics::resume(ECS::Registry& registry, ECS::EntityId channelId)
{
	Ref<FMod> fmod = FModStatics::getFModChecked(registry);
	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->isPaused = false;
	if (channel->value != nullptr)
	{
		channel->value->setPaused(channel->isPaused);
	}
}

void FModStatics::setVolume(ECS::Registry& registry, ECS::EntityId channelId, float volume)
{
	Ref<FMod> fmod = FModStatics::getFModChecked(registry);
	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	channel->volume = volume;
	if (channel->value != nullptr)
	{
		channel->value->setVolume(channel->volume);
	}
}

void FModStatics::stop(ECS::Registry& registry, ECS::EntityId channelId)
{
	Ref<FMod> fmod = FModStatics::getFModChecked(registry);
	Ref<FModChannel> channel = registry.get<FModChannel>(channelId);
	if (channel->value != nullptr)
	{
		channel->value->stop();
		return;
	}

	Ref<FModPlayQueue> queue = registry.getSingle<FModPlayQueue>();
	queue->value.remove(channelId);
	registry.destroy(channelId);
}
