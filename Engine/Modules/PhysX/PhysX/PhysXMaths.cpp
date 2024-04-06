#include "PhysXMaths.h"

physx::PxVec3 Mani::PhysXMaths::toPxVec3(const glm::vec3& v)
{
	return physx::PxVec3(v.x, v.y, v.z);
}

physx::PxQuat Mani::PhysXMaths::toPxQuat(const glm::quat& q)
{
	return physx::PxQuat(q.x, q.y, q.z, q.w);
}

physx::PxTransform Mani::PhysXMaths::toPxTransform(const Transform& t)
{
	return physx::PxTransform(toPxVec3(t.position), toPxQuat(t.rotation));
}

glm::vec3 Mani::PhysXMaths::toglmVec3(const physx::PxVec3& v)
{
	return glm::vec3(v.x, v.y, v.z);
}

glm::quat Mani::PhysXMaths::toglmQuat(const physx::PxQuat& q)
{
	return glm::quat(q.w, q.x, q.y, q.z);
}
