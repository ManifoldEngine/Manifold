#pragma once

#include <string>
#include <vector>

namespace Mani
{
	constexpr std::string_view SHADERCONFIG_FILENAME = "ShaderConfig.json";

	struct ShaderConfig
	{
		std::string shaderCollectionRelativePath = "";
	};

	constexpr std::string_view SHADERCOLLECTION_FILENAME = "ShaderCollection.json";

	struct ShaderCollection
	{
		std::vector<std::string> paths;
	};
}