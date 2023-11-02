#include <OpenGL/OpenGL.h>
#include <string>

namespace ECSEngine
{
	struct OpenGLShader
	{
		OpenGLShader(const std::string_view& inName, const std::string_view& inVertexSource, const std::string_view& inFragmentSource);
		~OpenGLShader();

		bool compile();
		bool isCompiled() const;
		
		std::string name = "";
		std::string vertexSource = "";
		std::string fragmentSource = "";

		uint32_t shaderProgramId;

	private:
		static uint32_t compile(const std::string_view& source, int shaderType);
	};
}