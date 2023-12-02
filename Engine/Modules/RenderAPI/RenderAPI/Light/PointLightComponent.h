#pragma once

#include <glm/glm.hpp>

namespace Mani
{
	struct PointLightComponent
	{
        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(0.5f);
        glm::vec3 specular = glm::vec3(1.0f);

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
	};
}