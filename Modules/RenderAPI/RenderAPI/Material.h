#pragma once

#include <vector>
#include <memory>
#include <filesystem>
#include <glm/glm.hpp>

namespace Mani
{
	struct Material
	{
		std::string name;

		glm::vec4 color = glm::vec4(1.f);

		std::string shaderPath;
		std::string diffusePath;
		std::string specularPath;

		float shininess = 32.f;
	};
}