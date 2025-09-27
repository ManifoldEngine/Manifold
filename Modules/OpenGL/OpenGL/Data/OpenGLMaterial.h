#pragma once

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

		std::string name;

		Vec4f color = VEC4F::ONE;

		ECS::EntityId shaderId;
		std::vector<Texture> textures;
		std::vector<std::pair<std::string, ShaderType>> shaderParameters;
	};
}