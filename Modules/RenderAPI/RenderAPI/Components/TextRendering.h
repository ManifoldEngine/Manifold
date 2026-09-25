#pragma once

#include <Core/CoreFwd.h>
#include <RenderAPI/Shader.h>
#include <string>

namespace Mani
{
	struct TextRendering
	{
		Mani::EntityId fontId = Mani::INVALID_ID;
		std::string_view text = "";
		float height = 16.f;
	};
}