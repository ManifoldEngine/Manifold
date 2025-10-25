#include "ResourceLoader_FModStream.h"

#include <Resources/Resources.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModStream.h>
#include <FMod/Resources/FModSound.h>

#include <fmod_errors.h>

using namespace Mani;

ECS::ComponentId ResourceLoader_FModStream::getComponentId(const ECS::Registry& registry) const
{
    return registry.getComponentId<Resource<FModStream>>();
}

bool ResourceLoader_FModStream::load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    FMod* fmod = registry.getSingle<FMod>();
    MANI_ASSERT(fmod != nullptr, "FModSystem is expected to initialized at this point.");
    FMOD::System* system = fmod->system;

    Resource<FModStream>& resource = registry.getRef<Resource<FModStream>>(resourceId);
    Resource<FModSound>& soundResource = *registry.add<Resource<FModSound>>(resourceId);

    FMOD_RESULT result = system->createStream(absolutePath.string().c_str(), FMOD_DEFAULT, 0, &(soundResource.value.sound));

    if (result != FMOD_OK)
    {
        MANI_LOG_ERROR(LogFMod, "Failed to load {} : {}", absolutePath.string(), FMOD_ErrorString(result));
        return false;
    }

    soundResource.isReady = true;
    return true;
}
