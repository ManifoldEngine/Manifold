#include "ResourceLoader_FModSound.h"

#include <Resources/Resources.h>

#include <FMod/FMod.h>
#include <FMod/Resources/FModSound.h>

#include <fmod_errors.h>

using namespace Mani;

bool Mani::ResourceLoader_FModSound::load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const
{
    Ref<FMod> fmod = registry.getSingle<FMod>();

    Ref<Resource<FModSound>> resource = registry.get<Resource<FModSound>>(resourceId);
    FMOD_RESULT result = fmod->system->createSound(absolutePath.string().c_str(), FMOD_DEFAULT, 0, &(resource->value.sound));

    if (result != FMOD_OK)
    {
        MANI_LOG_ERROR(LogFMod, "Failed to load {} : {}", absolutePath.string(), FMOD_ErrorString(result));
        return false;
    }
    return true;
}
