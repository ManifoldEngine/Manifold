#pragma once

#include <RenderAPI/AssetId.h>
#include <glm/glm.hpp>
#include <vector>

namespace ECSEngine
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoordinate;
	};

	struct Mesh
	{
		AssetId id;
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
}