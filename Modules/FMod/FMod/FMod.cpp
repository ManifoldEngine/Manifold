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

FModPlayQueue& getFModPlayQueueChecked(ECS::Registry& registry)
{
	FModPlayQueue* queue = registry.getSingle<FModPlayQueue>();
	MANI_ASSERT(queue != nullptr, "FModPlaySystem isn't initialized.");
	return *queue;
}

FMod& FModStatics::getFModChecked(ECS::Registry& registry)
{
	FMod* fmod = registry.getSingle<FMod>();
	MANI_ASSERT(fmod != nullptr, "FModSystem isn't initialized.");
	return *fmod;
}

ECS::EntityId FModStatics::play(ECS::Registry& registry, ECS::EntityId resourceId)
{
	MANI_ASSERT(registry.has<Resource<FModSound>>(resourceId) || registry.has<Resource<FModStream>>(resourceId), "trying to play something that is not a FModSound");
	FModPlayQueue& queue = getFModPlayQueueChecked(registry);
	ECS::EntityId entityId = createChannel(registry);

	FModChannel& channel = registry.getRef<FModChannel>(entityId);
	channel.resourceId = resourceId;

	registry.add<FModOneShot>(entityId);
	
	queue.value.enqueue(entityId);
	return entityId;
}

ECS::EntityId FModStatics::loop(ECS::Registry& registry, ECS::EntityId resourceId)
{
	MANI_ASSERT(registry.has<Resource<FModSound>>(resourceId) || registry.has<Resource<FModStream>>(resourceId), "trying to play something that is not a FModSound");
	FModPlayQueue& queue = getFModPlayQueueChecked(registry);
	ECS::EntityId entityId = createChannel(registry);

	FModChannel& channel = registry.getRef<FModChannel>(entityId);
	channel.resourceId = resourceId;
	
	queue.value.enqueue(entityId);
	return entityId;
}

void FModStatics::pause(ECS::Registry& registry, ECS::EntityId entityId)
{
	FMod& fmod = FModStatics::getFModChecked(registry);
	FModChannel& channel = registry.getRef<FModChannel>(entityId);
	channel.isPaused = true;
	if (channel.value != nullptr)
	{
		channel.value->setPaused(channel.isPaused);
	}
}

void FModStatics::resume(ECS::Registry& registry, ECS::EntityId entityId)
{
	FMod& fmod = FModStatics::getFModChecked(registry);
	FModChannel& channel = registry.getRef<FModChannel>(entityId);
	channel.isPaused = false;
	if (channel.value != nullptr)
	{
		channel.value->setPaused(channel.isPaused);
	}
}

void FModStatics::setVolume(ECS::Registry& registry, ECS::EntityId entityId, float volume)
{
	FMod& fmod = FModStatics::getFModChecked(registry);
	FModChannel& channel = registry.getRef<FModChannel>(entityId);
	channel.volume = volume;
	if (channel.value != nullptr)
	{
		channel.value->setVolume(channel.volume);
	}
}

void FModStatics::stop(ECS::Registry& registry, ECS::EntityId entityId)
{
	FMod& fmod = FModStatics::getFModChecked(registry);
	FModChannel& channel = registry.getRef<FModChannel>(entityId);
	if (channel.value != nullptr)
	{
		channel.value->stop();
		return;
	}

	FModPlayQueue& queue = getFModPlayQueueChecked(registry);
	queue.value.remove(entityId);
	registry.destroy(entityId);
}
