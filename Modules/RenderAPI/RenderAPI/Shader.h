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
		constexpr std::string_view MANI_TEXTURE_0 = "mani_texture_0";
		constexpr std::string_view MANI_TEXTURE_1 = "mani_texture_1";
		constexpr std::string_view MANI_TEXTURE_2 = "mani_texture_2";
		constexpr std::string_view MANI_TEXTURE_3 = "mani_texture_3";
		constexpr std::string_view MANI_TEXTURE_4 = "mani_texture_4";

		constexpr std::string_view MANI_MODEL			= "mani_model";
		constexpr std::string_view MANI_NORMALMATRIX	= "mani_normalMatrix";
		constexpr std::string_view MANI_VIEW			= "mani_view";
		constexpr std::string_view MANI_PROJECTION		= "mani_projection";
		constexpr std::string_view MANI_UI_PROJECTION	= "mani_ui_projection";
		constexpr std::string_view MANI_CAMERA_POSITION	= "mani_camera_position";

		constexpr std::string_view MANI_COLOR		= "mani_color";

		constexpr std::string_view MANI_SPRITE_TEXTURE		= "mani_sprite_texture";
		constexpr std::string_view MANI_SPRITE_TPU			= "mani_sprite_tpu";
		constexpr std::string_view MANI_SPRITE_WORLD_SIZE	= "mani_sprite_world_size";
		constexpr std::string_view MANI_SPRITE_TEXTURE_SIZE = "mani_sprite_texture_size";
	}
}