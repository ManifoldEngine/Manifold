#pragma once

#include <Core/Interfaces/IJsonAsset.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <filesystem>
#include <string_view>

namespace Mani
{
    const std::string_view LogScene = "Scene";

	struct Scene : public IJsonAsset
	{
		struct Node
		{
			size_t id = UINT64_MAX;

			std::filesystem::path meshAsset;

			glm::vec3 localPosition = glm::vec3(0.f);
			glm::quat localRotation = glm::quat(1.0f, 0.f, 0.f, 0.f);
			glm::vec3 localScale = glm::vec3(1.f);
		};

		std::vector<Node> nodes;

		virtual void parse(const std::string_view& content) override;
		virtual std::string toJson() override;
	};
}