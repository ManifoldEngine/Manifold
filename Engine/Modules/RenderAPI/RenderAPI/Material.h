#pragma once

#include <RenderAPI/AssetId.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/Texture.h>
#include <vector>
#include <memory>

namespace ECSEngine
{
	struct Material
	{
		AssetId id;

		glm::vec4 color;

		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture> diffuse;
		std::shared_ptr<Texture> specular;

		float shininess = 32.f;
	};
}