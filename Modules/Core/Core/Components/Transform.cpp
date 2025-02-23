#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Mani;

bool Mani::Transform::hasParent() const
{
	return ECS::isValid(parentId);
}

glm::mat4 Transform::calculateModelMatrix() const
{
	return	glm::translate(glm::mat4(1.0f), position) *
			glm::toMat4(rotation) *
			glm::scale(glm::mat4(1.0f), scale);
}

glm::vec3 Transform::forward() const
{
	glm::vec3 forward = glm::rotate(rotation, glm::vec3(0.f, 0.f, 1.f));
	return glm::normalize(forward);
}

glm::vec3 Transform::up() const
{
	glm::vec3 up = glm::rotate(rotation, glm::vec3(0.f, 1.f, 0.f));
	return glm::normalize(up);
}

glm::vec3 Transform::right() const
{
	glm::vec3 right = glm::rotate(rotation, glm::vec3(1.f, 0.f, 0.f));
	return glm::normalize(right);
}
