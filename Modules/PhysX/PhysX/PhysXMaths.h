#pragma once

#include <Core/CoreFwd.h>
#include <foundation/PxVec3.h>
#include <foundation/PxQuat.h>
#include <foundation/PxTransform.h>
#include <glm/fwd.hpp>

namespace Mani
{
	class PhysXMaths
	{
	public:
		static physx::PxVec3 toPxVec3(const glm::vec3& v);
		static physx::PxQuat toPxQuat(const glm::quat& q);
		static physx::PxTransform toPxTransform(const Transform& t);

		static glm::vec3 toglmVec3(const physx::PxVec3& v);
		static glm::quat toglmQuat(const physx::PxQuat& q);
	};
}