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

	namespace ShaderConstants
	{
		constexpr std::string_view MANI_TEXTURE_0 = "mani_texture_0";
	}
}