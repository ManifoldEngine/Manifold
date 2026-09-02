#include "FModPlaySystem.h"

#include <Resources/Resources.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModSound.h>
#include <FMod/Resources/FModStream.h>
#include <FMod/Components/FModPlayQueue.h>
#include <FMod/Components/FModChannel.h>

using namespace Mani;

bool FModPlaySystem::shouldTick(const ECS::Registry& registry) const
{
    Ref<const FModPlayQueue> queue = registry.getSingle<FModPlayQueue>();
    return !queue->value.isEmpty();
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
    Ref<FMod> fmod = FModControls::getFModChecked(registry);

    Ref<FModPlayQueue> queue = registry.getSingle<FModPlayQueue>();
    List<EntityId> channelsPendingLoading;
    while (!queue->value.isEmpty())
    {
        EntityId channelId = queue->value.dequeue();
        Ref<FModChannel> channel = registry.get<FModChannel>(channelId);

        if (!Resources::isReady(registry, channel->resourceId))
        {
            channelsPendingLoading.add(channelId);
            continue;
        }

        Resource<FModSound>& resource = registry.getPinned<Resource<FModSound>>(channel->resourceId);
        constexpr FMOD::ChannelGroup* group = nullptr;
        constexpr bool isPaused = false;
        fmod->system->playSound(resource.value.sound, group, isPaused, &channel->value);
        if (channel->isLooping)
        {
            channel->value->setMode(FMOD_LOOP_NORMAL);
        }

        channel->value->setPaused(channel->isPaused);
        channel->value->setVolume(channel->volume);
    }

    // channels that are pending loading are put back into the queue
    for (const EntityId entityId : channelsPendingLoading)
    {
        queue->value.enqueue(entityId);
    }
}
