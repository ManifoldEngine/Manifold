#pragma once

#include <Core/ECS/Entity.h>
#include <ManiMaths/Fwd.h>
#include <string>

namespace Mani
{
	struct OpenGLMaterial
	{
		std::string name;

		Vec4f color = VEC4F::ONE;

		ECS::EntityId shaderId;
		ECS::EntityId diffuseId;
		ECS::EntityId specularId;

		float shininess = 32.f;
	};
}