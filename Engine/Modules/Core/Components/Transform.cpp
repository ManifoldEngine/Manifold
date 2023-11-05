#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace ECSEngine;

glm::mat4 Transform::calculate()
{
	transform = glm::mat4(1.0f);
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, rotation.x, glm::vec3(1.f, 0.f, 0.f));
	transform = glm::rotate(transform, rotation.y, glm::vec3(0.f, 1.f, 0.f));
	transform = glm::rotate(transform, rotation.z, glm::vec3(0.f, 0.f, 1.f));
	transform = glm::scale(transform, scale);
	return transform;
}

glm::mat4 Transform::getTransform() const
{
	return transform;
}
