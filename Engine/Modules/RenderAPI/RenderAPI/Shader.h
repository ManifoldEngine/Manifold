#pragma once

#include <RenderAPI/AssetId.h>
#include <string>

namespace ECSEngine
{
	struct Shader
	{
		AssetId id;
		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";
	};
}