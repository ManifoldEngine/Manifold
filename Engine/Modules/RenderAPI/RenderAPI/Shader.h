#pragma once

#include <Assets/IJsonAsset.h>
#include <string>

namespace ECSEngine
{
	struct Shader : public IJsonAsset
	{
		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";

		virtual void parse(const std::string_view& content) override;
		virtual std::string toJson() override;
	};
}