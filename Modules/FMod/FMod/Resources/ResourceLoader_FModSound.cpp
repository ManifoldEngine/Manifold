#include "ResourceLoader_FModSound.h"

#include <Resources/Resources.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModSound.h>

#include <fmod_errors.h>

using namespace Mani;

bool Mani::ResourceLoader_FModSound::load(ECS::Registry& registry, const Path& absolutePath, EntityId resourceId, uint32_t tag) const
{
    Ref<FMod> fmod = registry.getSingle<FMod>();

    Resource<FModSound>& resource = registry.getPinned<Resource<FModSound>>(resourceId);
    FMOD_RESULT result = fmod->system->createSound(absolutePath.string().c_str(), FMOD_DEFAULT, 0, &(resource.value.sound));

    if (result != FMOD_OK)
    {
        MANI_LOG_ERROR(LogFMod, "Failed to load {} : {}", absolutePath.string(), FMOD_ErrorString(result));
        return false;
    }
    return true;
}

bool Mani::ResourceLoader_FModSound::unload(ECS::Registry& registry, EntityId resourceId) const
{
    Resource<FModSound>& sound = registry.getPinned<Resource<FModSound>>(resourceId);
    MANI_ASSERT(sound.value.sound != nullptr, "trying to unload a sound that is null");
    sound.value.sound->release();
    return true;
}
