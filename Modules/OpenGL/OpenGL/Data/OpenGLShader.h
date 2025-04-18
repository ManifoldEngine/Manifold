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

		void setFloat(const std::string_view& name, float value);
		void setFloat2(const std::string_view& name, float x, float y);
		void setFloat3(const std::string_view& name, float x, float y, float z);
		void setFloat4(const std::string_view& name, float x, float y, float z, float w);

		void setInt(const std::string_view& name, int value);
		void setInt2(const std::string_view& name, int x, int y);
		void setInt3(const std::string_view& name, int x, int y, int z);
		void setInt4(const std::string_view& name, int x, int y, int z, int w);

		void setFloatMatrix3(const std::string_view& name, const float* value);
		void setFloatMatrix4(const std::string_view& name, const float* value);
		
		void setBool(const std::string_view& name, bool value);

		void setTextureSlot(const std::string_view& name, uint32_t slot);

		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";

		uint32_t shaderProgramId;

	private:
		static uint32_t compile(const std::string_view& source, int shaderType);
	};
}