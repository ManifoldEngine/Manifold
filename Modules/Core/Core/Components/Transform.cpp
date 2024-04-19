#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>

using namespace Mani;
using namespace nlohmann;

bool Mani::Transform::hasParent() const
{
	return isValid(parentId);
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

void Mani::Transform::parse(const std::string_view& content)
{
	position = glm::vec3();
	localPosition = glm::vec3();

	rotation = glm::quat();
	localRotation = glm::quat();
	
	scale = glm::vec3();
	localScale = glm::vec3();

	if (content.empty())
	{
		return;
	}

	json object = json::parse(content);

	parentId = object["parentId"];

	std::vector<float> rawData = object["trasnform"];

	position.x = rawData[0];
	position.y = rawData[1];
	position.z = rawData[2];

	localPosition.x = rawData[3];
	localPosition.y = rawData[4];
	localPosition.z = rawData[5];

	rotation.x = rawData[6];
	rotation.y = rawData[7];
	rotation.z = rawData[8];
	rotation.w = rawData[9];

	localRotation.x = rawData[10];
	localRotation.y = rawData[11];
	localRotation.z = rawData[12];
	localRotation.w = rawData[13];

	scale.x = rawData[14];
	scale.y = rawData[15];
	scale.z = rawData[16];

	localScale.x = rawData[17];
	localScale.y = rawData[18];
	localScale.z = rawData[19];
}

std::string Mani::Transform::toJson()
{
	json output;

	std::vector<float> rawData;

	rawData.push_back(position.x);
	rawData.push_back(position.y);
	rawData.push_back(position.z);

	rawData.push_back(localPosition.x);
	rawData.push_back(localPosition.y);
	rawData.push_back(localPosition.z);

	rawData.push_back(rotation.x);
	rawData.push_back(rotation.y);
	rawData.push_back(rotation.z);
	rawData.push_back(rotation.w);

	rawData.push_back(localRotation.x);
	rawData.push_back(localRotation.y);
	rawData.push_back(localRotation.z);
	rawData.push_back(localRotation.w);

	rawData.push_back(scale.x);
	rawData.push_back(scale.y);
	rawData.push_back(scale.z);

	rawData.push_back(localScale.x);
	rawData.push_back(localScale.y);
	rawData.push_back(localScale.z);

	output["transform"] = rawData;
	output["parentId"] = parentId;

	return output.dump();
}
