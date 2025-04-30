#pragma once

#include <ManiMaths/Fwd.h>

namespace Mani
{
	struct Spotlight
	{
        float cutOff = Math::cos(Math::degToRad(12.5f));
        float outterCutOff = Math::cos(Math::degToRad(17.5f));

        Vec3f ambient = { 0.2f, 0.2f, 0.2f };
        Vec3f diffuse = { 0.5f, 0.5f, 0.5f };
        Vec3f specular = { 1.0f, 1.0f, 1.0f };
	};
}