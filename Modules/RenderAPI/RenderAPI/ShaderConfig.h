#pragma once

#include <Core/Containers/List.h>
#include <string>

namespace Mani
{
	constexpr std::string_view SHADERCOLLECTION_FILENAME = "ShaderCollection.json";

	struct ShaderCollection
	{
		List<std::string> paths;
	};
}