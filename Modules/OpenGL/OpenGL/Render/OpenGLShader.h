#include <OpenGL/OpenGL.h>
#include <string>

namespace Mani
{
	struct OpenGLShader
	{
		OpenGLShader(const std::string_view& inName, const std::string_view& inVertexSource, const std::string_view& inFragmentSource);
		~OpenGLShader();

		bool compile();
		bool isCompiled() const;
		void use() const;

		void setFloat(const char* name, float value);
		void setFloat2(const char* name, float x, float y);
		void setFloat3(const char* name, float x, float y, float z);
		void setFloat4(const char* name, float x, float y, float z, float w);

		void setInt(const char* name, int value);
		void setInt2(const char* name, int x, int y);
		void setInt3(const char* name, int x, int y, int z);
		void setInt4(const char* name, int x, int y, int z, int w);

		void setFloatMatrix3(const char* name, float* value);
		void setFloatMatrix4(const char* name, float* value);
		
		void setBool(const char* name, bool value);

		void setTextureSlot(const char* name, uint32_t slot);

		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";

		uint32_t shaderProgramId;

	private:
		static uint32_t compile(const std::string_view& source, int shaderType);
	};
}