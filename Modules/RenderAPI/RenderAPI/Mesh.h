#pragma once

#include <Core/Containers/List.h>
#include <ManiMaths/Fwd.h>
#include <string>

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
		List<Vertex> vertices;
		List<unsigned int> indices;
	};
}