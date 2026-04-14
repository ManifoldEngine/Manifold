#include "ResourceLoader_FModStream.h"

#include <Resources/Resources.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModStream.h>
#include <FMod/Resources/FModSound.h>

#include <fmod_errors.h>

using namespace Mani;

bool ResourceLoader_FModStream::load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    Ref<FMod> fmod = registry.getSingle<FMod>();
    
    Ref<Resource<FModStream>> resource = registry.get<Resource<FModStream>>(resourceId);
    

    FMOD_RESULT result = fmod->system->createStream(absolutePath.string().c_str(), FMOD_DEFAULT, 0, &(resource->value.sound));
    const bool success = result != FMOD_OK;
    if (!success)
    {
        MANI_LOG_ERROR(LogFMod, "Failed to load {} : {}", absolutePath.string(), FMOD_ErrorString(result));
    }

    return success;
}

void ResourceLoader_FModStream::postLoad(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    // add the FModSound resource so it can be processed like a normal sound.
    Ref<Resource<FModStream>> resource = registry.get<Resource<FModStream>>(resourceId);
    Ref<Resource<FModSound>> soundResource = registry.add<Resource<FModSound>>(resourceId);
    soundResource->value.sound = resource->value.sound;
}
