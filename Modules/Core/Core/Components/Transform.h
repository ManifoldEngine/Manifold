#pragma once

#include <Core/Core.h>
#include <ECS/Entity.h>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	struct Transform
	{
		Vec3f position	= VEC3F::ZERO;
		Quatf rotation	= QUATF::IDENTITY;
		Vec3f scale		= VEC3F::ONE;
		
		Mat4f calculateModelMatrix() const;

		Vec3f forward() const;
		Vec3f up() const;
		Vec3f right() const;
	};
}