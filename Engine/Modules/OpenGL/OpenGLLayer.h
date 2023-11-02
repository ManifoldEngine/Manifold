#pragma once

#include "OpenGL.h"
#include "OpenGLVertexArray.h"
#include "OpenGLBuffer.h"
#include <Core/Interfaces/ITickable.h>
#include <memory>
#include <vector>

namespace ECSEngine
{
	class OpenGLVertexArray;
	class OpenGLVertexBuffer;
	class OpenGLIndexBuffer;

	class OpenGLLayer : public ITickable
	{
	public:
		void tick(float deltaTime) override;

		void initialize();
		void deinitialize();
	private:
		const std::string_view vertexShaderSource = "#version 330 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\n";

		const std::string_view fragmentShaderOrangeSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\n";

		const std::string_view fragmentShaderWhiteSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
			"}\n";

		struct ShaderProgram
		{
			ShaderProgram(const std::string_view& inVertexShaderSource, const std::string_view& inFragmentShaderSource)
				: vertexShaderSource(inVertexShaderSource), fragmentShaderSource(inFragmentShaderSource)
			{};

			std::string_view vertexShaderSource;
			std::string_view fragmentShaderSource;

			unsigned int shaderProgramId = -1;
		};

		ShaderProgram orangeShaderProgram = ShaderProgram(vertexShaderSource, fragmentShaderOrangeSource);
		
		std::unique_ptr<OpenGLVertexArray> m_pHalfScreenSquareVertexArray;
		std::unique_ptr<OpenGLVertexArray> m_pSquareVertexArray;

		static bool initializeShaderProgram(ShaderProgram& context);
		static uint32_t compileShader(const std::string_view& source, int shaderType);
		static uint32_t linkShaderProgram(uint32_t vertexShaderId, uint32_t fragmentShaderId);
	};
}