#pragma once

#include <string>
#include <variant>
#include <ManiMaths/Fwd.h>

namespace Mani
{
	using ShaderType = std::variant<float, Vec2f, Vec3f, Vec4f, int, Vec2i, Vec3i, Vec4i, Mat3f, Mat4f, bool>;

	struct Shader
	{
		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";
	};

	namespace ShaderNames
	{
		const std::string MANI_TEXTURE_0 = "mani_texture_0";
		const std::string MANI_TEXTURE_1 = "mani_texture_1";
		const std::string MANI_TEXTURE_2 = "mani_texture_2";
		const std::string MANI_TEXTURE_3 = "mani_texture_3";
		const std::string MANI_TEXTURE_4 = "mani_texture_4";

		const std::string MANI_MODEL		= "mani_model";
		const std::string MANI_NORMALMATRIX	= "mani_normalMatrix";
		const std::string MANI_VIEW			= "mani_view";
		const std::string MANI_PROJECTION	= "mani_projection";

		const std::string MANI_COLOR		= "mani_color";
	}
}