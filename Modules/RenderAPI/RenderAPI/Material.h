#pragma once

#include <Core/Vec.h>
#include <Core/Containers/List.h>
#include <RenderAPI/Shader.h>
#include <string>

namespace Mani
{
	struct ShaderParam_Texture { std::string_view key = ""; std::string_view path = ""; };
	struct ShaderParam_float { std::string_view key = ""; float value = 0.f; };
	struct ShaderParam_Vec2f { std::string_view key = ""; Vec2f value = VEC2F::ZERO; };
	struct ShaderParam_Vec3f { std::string_view key = ""; Vec3f value = VEC3F::ZERO; };
	struct ShaderParam_Vec4f { std::string_view key = ""; Vec4f value = VEC4F::ZERO; };
	struct ShaderParam_int { std::string_view key = ""; int value = 0; };
	struct ShaderParam_Vec2i { std::string_view key = ""; Vec2i value = VEC2I::ZERO; };
	struct ShaderParam_Vec3i { std::string_view key = ""; Vec3i value = VEC3I::ZERO; };
	struct ShaderParam_Vec4i { std::string_view key = ""; Vec4i value = VEC4I::ZERO; };
	struct ShaderParam_Mat3f { std::string_view key = ""; Mat3f value = MAT4F::IDENTITY; };
	struct ShaderParam_Mat4f { std::string_view key = ""; Mat4f value = MAT3F::IDENTITY; };
	struct ShaderParam_bool { std::string_view key = ""; bool value = false; };

	struct Material
	{
		std::string_view name;

		std::string_view shaderPath = "";
		List<ShaderParam_Texture> textures;
		
		List<ShaderParam_float> shaderParam_float_values;
		List<ShaderParam_Vec2f> shaderParam_Vec2f_values;
		List<ShaderParam_Vec3f> shaderParam_Vec3f_values;
		List<ShaderParam_Vec4f> shaderParam_Vec4f_values;
		List<ShaderParam_int> shaderParam_int_values;
		List<ShaderParam_Vec2i> shaderParam_Vec2i_values;
		List<ShaderParam_Vec3i> shaderParam_Vec3i_values;
		List<ShaderParam_Vec4i> shaderParam_Vec4i_values;
		List<ShaderParam_Mat3f> shaderParam_Mat3f_values;
		List<ShaderParam_Mat4f> shaderParam_Mat4f_values;
		List<ShaderParam_bool> shaderParam_bool_values;
	};
}