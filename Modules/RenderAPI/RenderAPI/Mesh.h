#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Mani
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	};

	struct Mesh
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}