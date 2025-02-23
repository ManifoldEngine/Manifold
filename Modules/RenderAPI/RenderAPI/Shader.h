#pragma once

#include <string>

namespace Mani
{
	struct Shader
	{
		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";
	};
}