#pragma once

#include <ManiMaths/Fwd.h>

namespace Mani
{
	struct Material
	{
		std::string name;

		Vec4f color = VEC4F::ONE;

		std::string shaderPath;
		std::string diffusePath;
		std::string specularPath;

		float shininess = 32.f;
	};
}