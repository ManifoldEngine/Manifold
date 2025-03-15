#pragma once

#include <string>
#include <vector>
#include <ManiMaths/Fwd.h>

namespace Mani
{
    const std::string_view LogScene = "Scene";

	struct Scene
	{
		struct Node
		{
			size_t id = UINT64_MAX;

			std::string meshAsset;

			Vec3f localPosition = VEC3F::ZERO;
			Quatf localRotation = QUATF::IDENTITY;
			Vec3f localScale = VEC3F::ONE;
		};

		std::vector<Node> nodes;
	};
}