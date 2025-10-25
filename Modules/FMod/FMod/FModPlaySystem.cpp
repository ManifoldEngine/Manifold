#include "FModPlaySystem.h"

#include <Resources/Components/Resource.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModSound.h>
#include <FMod/Resources/FModStream.h>
#include <FMod/Components/FModPlayQueue.h>
#include <FMod/Components/FModChannel.h>
#include <FMod/Components/FModOneShot.h>

using namespace Mani;

bool FModPlaySystem::shouldTick(const ECS::Registry& registry) const
{
    const FModPlayQueue& queue = *registry.getSingle<FModPlayQueue>();
    return !queue.value.isEmpty();
}

void FModPlaySystem::onInitialize(ECS::Registry& registry, World& world)
{
    registry.addSingle<FModPlayQueue>();
}

void FModPlaySystem::onDeinitialize(ECS::Registry& registry, World& world)
{
    registry.removeSingle<FModPlayQueue>();
}

void FModPlaySystem::tick(ECS::Registry& registry)
{
    FMod& fmod = FModStatics::getFModChecked(registry);

    FModPlayQueue& queue = *registry.getSingle<FModPlayQueue>();
    List<ECS::EntityId> channelsPendingLoading;
    while (!queue.value.isEmpty())
    {
        ECS::EntityId channelId = queue.value.dequeue();
        FModChannel& channel = registry.getRef<FModChannel>(channelId);

        Resource<FModSound>& resource = registry.getRef<Resource<FModSound>>(channel.resourceId);
        if (!resource.isReady)
        {
            channelsPendingLoading.add(channelId);
            continue;
        }

        constexpr FMOD::ChannelGroup* group = nullptr;
        constexpr bool isPaused = false;
        fmod.system->playSound(resource.value.sound, group, isPaused, &channel.value);
        if (!registry.has<FModOneShot>(channelId))
        {
            channel.value->setMode(FMOD_LOOP_NORMAL);
        }

        channel.value->setPaused(channel.isPaused);
        channel.value->setVolume(channel.volume);
    }

    // channels that are pending loading are put back into the queue
    for (const ECS::EntityId entityId : channelsPendingLoading)
    {
        queue.value.enqueue(entityId);
    }
}
