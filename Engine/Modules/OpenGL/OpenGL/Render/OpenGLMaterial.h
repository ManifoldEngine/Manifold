#pragma once

#include <string>
#include <glm/glm.hpp>


namespace ECSEngine
{
	struct OpenGLMaterial
	{
		std::string name;

		glm::vec4 color;

		std::string shader;
		std::string diffuse;
		std::string specular;

		float shininess = 32.f;
	};
}