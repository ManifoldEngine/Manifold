#pragma once

#include <Assets/IJsonAsset.h>
#include <vector>
#include <memory>
#include <filesystem>
#include <glm/glm.hpp>

namespace Mani
{
	struct Material : public IJsonAsset
	{
		std::string name;

		glm::vec4 color = glm::vec4(1.f);

		std::filesystem::path shaderPath;
		std::filesystem::path diffusePath;
		std::filesystem::path specularPath;

		float shininess = 32.f;

		virtual void parse(const std::string_view& content) override;
		virtual std::string toJson() override;
	};
}