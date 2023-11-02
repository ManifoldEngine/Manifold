#include <OpenGL/OpenGL.h>
#include <Core/System/System.h>
#include <unordered_map>
#include <memory>

namespace ECSEngine
{
	struct OpenGLShader;

	class OpenGLShaderSystem : public SystemBase
	{
	public:
		bool loadShaderFromPath(const std::string& filePath);
		bool loadShaderFromSource(const std::string_view& name, const std::string_view& vertexSource, const std::string_view& fragmentSource);

		std::shared_ptr<const OpenGLShader> getShader(const std::string_view& name) const;

	private:
		std::unordered_map<std::string_view, std::shared_ptr<OpenGLShader>> m_shaders;
	};
}