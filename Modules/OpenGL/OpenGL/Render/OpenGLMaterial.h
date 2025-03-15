#pragma once

#include <string>
#include <ManiMaths/Fwd.h>


namespace Mani
{
	struct OpenGLMaterial
	{
		std::string name;

		Vec4f color = VEC4F::ONE;

		std::string shader;
		std::string diffuse;
		std::string specular;

		float shininess = 32.f;
	};
}