#pragma once

#include <Core/Interfaces/IJsonAsset.h>
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

	struct Mesh : public IJsonAsset
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		virtual void parse(const std::string_view& json) override;
		virtual std::string toJson() override;
	};
}