#pragma once

#include <Core/Vec.h>
#include <RenderAPI/Shader.h>
#include <vector>
#include <string>

namespace Mani
{
	struct ShaderParam_Texture { std::string key = ""; std::string path = ""; };
	struct ShaderParam_float { std::string key = ""; float value = 0.f; };
	struct ShaderParam_Vec2f { std::string key = ""; Vec2f value = VEC2F::ZERO; };
	struct ShaderParam_Vec3f { std::string key = ""; Vec3f value = VEC3F::ZERO; };
	struct ShaderParam_Vec4f { std::string key = ""; Vec4f value = VEC4F::ZERO; };
	struct ShaderParam_int { std::string key = ""; int value = 0; };
	struct ShaderParam_Vec2i { std::string key = ""; Vec2i value = VEC2I::ZERO; };
	struct ShaderParam_Vec3i { std::string key = ""; Vec3i value = VEC3I::ZERO; };
	struct ShaderParam_Vec4i { std::string key = ""; Vec4i value = VEC4I::ZERO; };
	struct ShaderParam_Mat3f { std::string key = ""; Mat3f value = MAT4F::IDENTITY; };
	struct ShaderParam_Mat4f { std::string key = ""; Mat4f value = MAT3F::IDENTITY; };
	struct ShaderParam_bool { std::string key = ""; bool value = false; };

	struct Material
	{
		std::string name;

		Vec4f color = VEC4F::ONE;

		std::string shaderPath = "";
		std::vector<ShaderParam_Texture> textures;
		
		std::vector<ShaderParam_float> shaderParam_float_values;
		std::vector<ShaderParam_Vec2f> shaderParam_Vec2f_values;
		std::vector<ShaderParam_Vec3f> shaderParam_Vec3f_values;
		std::vector<ShaderParam_Vec4f> shaderParam_Vec4f_values;
		std::vector<ShaderParam_int> shaderParam_int_values;
		std::vector<ShaderParam_Vec2i> shaderParam_Vec2i_values;
		std::vector<ShaderParam_Vec3i> shaderParam_Vec3i_values;
		std::vector<ShaderParam_Vec4i> shaderParam_Vec4i_values;
		std::vector<ShaderParam_Mat3f> shaderParam_Mat3f_values;
		std::vector<ShaderParam_Mat4f> shaderParam_Mat4f_values;
		std::vector<ShaderParam_bool> shaderParam_bool_values;
	};
}