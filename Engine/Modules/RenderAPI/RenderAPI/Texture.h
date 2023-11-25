#pragma once

#include <RenderAPI/AssetId.h>

namespace ECSEngine
{
	struct Texture
	{
		AssetId id;
		// todo: placeholder. we should be able to query the file through an assetsystem.
		std::string path;
	};
}