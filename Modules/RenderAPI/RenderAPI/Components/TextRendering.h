#pragma once

#include <Core/CoreFwd.h>
#include <RenderAPI/Shader.h>
#include <string>

namespace Mani
{
	struct TextRendering
	{
		Mani::ECS::EntityId fontId = Mani::ECS::INVALID_ID;
		std::string_view text = "";
		float size = 16.f;
		Mani::Map<std::string_view, ShaderType> shaderParameters;
	};
}