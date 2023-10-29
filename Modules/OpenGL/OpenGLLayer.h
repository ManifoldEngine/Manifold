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
	class OpenGLBuffer;

	class OpenGLLayer : public ITickable
	{
	public:
		void tick(float deltaTime) override;

		void initialize();
		void deinitialize();
	private:
		struct RendererContext
		{
			const char* vertexShaderSource = "#version 330 core\n"
				"layout(location = 0) in vec3 aPos;\n"
				"void main()\n"
				"{\n"
				"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
				"}\n";

			const char* fragmentShaderSource = "#version 330 core\n"
				"out vec4 FragColor;\n"
				"void main()\n"
				"{\n"
				"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
				"}\n";

			unsigned int shaderProgramId = -1;
		};

		RendererContext rendererContext;

		std::unique_ptr<OpenGLVertexArray> m_pVertexArray;
		std::vector<std::shared_ptr<OpenGLBuffer>> m_pVertexBuffers;

		static bool initializeRenderConfig(RendererContext& context);
	};
}