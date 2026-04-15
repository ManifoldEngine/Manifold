#pragma once

#include <Core/Containers/Map.h>
#include <RenderAPI/Shader.h>
#include <string>

namespace Mani
{
	struct RenderContext
	{
		Map<std::string_view, ShaderType> shaderParameters;
	};
}