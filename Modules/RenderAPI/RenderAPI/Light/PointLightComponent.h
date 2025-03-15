#pragma once

#include <ManiMaths/Fwd.h>

namespace Mani
{
	struct PointLightComponent
	{
        Vec3f ambient = { 0.2f, 0.2f, 0.2f };
        Vec3f diffuse = { 0.5f, 0.5f, 0.5f };
        Vec3f specular = { 1.0f, 1.0f, 1.0f };

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
	};
}