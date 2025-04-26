#pragma once

#include <OpenGL/OpenGL.h>
#include <string>

namespace Mani
{
	struct OpenGLShader
	{
		OpenGLShader();
		OpenGLShader(const std::string_view& inName, const std::string_view& inVertexSource, const std::string_view& inFragmentSource);
		~OpenGLShader();

		bool compile();
		bool isCompiled() const;
		void use() const;

		void setFloat(const std::string_view& name, float value) const;
		void setFloat2(const std::string_view& name, float x, float y) const;
		void setFloat3(const std::string_view& name, float x, float y, float z) const;
		void setFloat4(const std::string_view& name, float x, float y, float z, float w) const;

		void setInt(const std::string_view& name, int value) const;
		void setInt2(const std::string_view& name, int x, int y) const;
		void setInt3(const std::string_view& name, int x, int y, int z) const;
		void setInt4(const std::string_view& name, int x, int y, int z, int w) const;

		void setFloatMatrix3(const std::string_view& name, const float* value) const;
		void setFloatMatrix4(const std::string_view& name, const float* value) const;
		
		void setBool(const std::string_view& name, bool value) const;

		void setTextureSlot(const std::string_view& name, uint32_t slot) const;

		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";

		uint32_t shaderProgramId;

	private:
		static uint32_t compile(const std::string_view& source, int shaderType);
	};
}