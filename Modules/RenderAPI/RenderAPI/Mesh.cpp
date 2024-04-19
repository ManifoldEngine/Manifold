#include "Mesh.h"

#include <Core/ManiAssert.h>

#include <nlohmann/json.hpp>

using namespace Mani;
using namespace nlohmann;

void Mesh::parse(const std::string_view& content)
{
	vertices.clear();
	indices.clear();
	name = "";

	if (content.empty())
	{
		return;
	}

	json object = json::parse(content);
	
	name = object["name"];

	std::vector<float> rawVertices = object["vertices"];
	MANI_ASSERT(rawVertices.size() % 8 == 0, "Vertices size should be divisible by 8");
	for (size_t i = 0; i < rawVertices.size(); i += 8)
	{
		Vertex vertex;
		vertex.position.x = rawVertices[i];
		vertex.position.y = rawVertices[i + 1];
		vertex.position.z = rawVertices[i + 2];

		vertex.normal.x = rawVertices[i + 3];
		vertex.normal.y = rawVertices[i + 4];
		vertex.normal.z = rawVertices[i + 5];

		vertex.textureCoordinate.x = rawVertices[i + 6];
		vertex.textureCoordinate.y = rawVertices[i + 7];
		vertices.push_back(vertex);
	}

	indices = object["indices"].get<std::vector<unsigned int>>();
}

std::string Mesh::toJson()
{
	json output;

	output["name"] = name;

	// export vertices
	std::vector<float> rawVertices;
	for (const Vertex& vertex : vertices)
	{
		rawVertices.push_back(vertex.position.x);
		rawVertices.push_back(vertex.position.y);
		rawVertices.push_back(vertex.position.z);

		rawVertices.push_back(vertex.normal.x);
		rawVertices.push_back(vertex.normal.y);
		rawVertices.push_back(vertex.normal.z);

		rawVertices.push_back(vertex.textureCoordinate.x);
		rawVertices.push_back(vertex.textureCoordinate.y);
	}

	output["vertices"] = rawVertices;

	std::vector<unsigned int> rawIndices;
	for (const unsigned int index : indices)
	{
		rawIndices.push_back(index);
	}

	output["indices"] = rawIndices;
    return output.dump();
}
