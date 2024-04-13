#pragma once

#include <glm/fwd.hpp>

namespace Mani
{
	struct Simplex
	{
		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;
		glm::vec3 d;

		unsigned char count = 0;
	};
}
