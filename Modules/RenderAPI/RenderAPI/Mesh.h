#pragma once

#include <vector>
#include <string>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	struct Vertex
	{
		Vec3f position;
		Vec3f normal;
		Vec2f textureCoordinate;
	};

	struct Mesh
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}