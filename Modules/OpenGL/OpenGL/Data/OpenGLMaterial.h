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
			std::string key = "";
			ECS::EntityId id;
		};

		struct ShaderParam
		{
			std::string key;
			ShaderType value;
		};

		std::string name;

		Vec4f color = VEC4F::ONE;

		ECS::EntityId shaderId;
		List<Texture> textures;
		List<ShaderParam> shaderParameters;
	};
}