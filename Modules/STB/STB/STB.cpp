#include "STB.h"

#include <STB/STBConfig.h>
#include <Resources/Resources.h>

Mani::ECS::EntityId Mani::STB::loadConfig(ECS::Registry& registry)
{
	return Resources::loadSync<STB::Config>(registry, Mani::FileSystem::getConfigFilePath());
}
