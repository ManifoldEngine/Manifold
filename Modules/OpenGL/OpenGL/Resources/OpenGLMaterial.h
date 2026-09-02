#pragma once

#include <Core/Containers/List.h>
#include <Core/ECS/Entity.h>
#include <RenderAPI/Shader.h>
#include <ManiMaths/Fwd.h>
#include <string>

namespace Mani
{
	struct OpenGLMaterial
	{
		struct Texture
		{
			std::string_view key = "";
			EntityId id = INVALID_ID;
		};

		struct ShaderParam
		{
			std::string_view key = "";
			ShaderType value;
		};

		std::string_view name;

		Vec4f color = VEC4F::ONE;

		EntityId shaderId;
		List<Texture> textures;
		List<ShaderParam> shaderParameters;
	};
}