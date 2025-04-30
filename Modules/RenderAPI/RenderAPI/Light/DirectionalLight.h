#pragma once

#include <ManiMaths/Fwd.h>

namespace Mani
{
	struct DirectionalLight
	{
		Vec3f direction = { -0.2f, -1.0f, -0.3f };

		Vec3f ambient = { 0.2f, 0.2f, 0.2f };
		Vec3f diffuse = { 0.5f, 0.5f, 0.5f };
		Vec3f specular = { 1.0f, 1.0f, 1.0f };
	};
}