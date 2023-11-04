#pragma once

#include "OpenGL.h"
#include "OpenGLVertexArray.h"
#include "OpenGLBuffer.h"
#include <Core/Interfaces/ITickable.h>
#include <memory>
#include <vector>

namespace ECSEngine
{
	class SystemContainer;
	class OpenGLVertexArray;
	class OpenGLIndexBuffer;
	struct OpenGLShader;

	class OpenGLLayer : public ITickable
	{
	public:
		void tick(float deltaTime) override;

		void initialize(SystemContainer& systemContainer);
		void deinitialize();
	private:

		std::unique_ptr<OpenGLVertexArray> m_pSquareVertexArray;
		std::unique_ptr<OpenGLIndexBuffer> m_pIndexBuffer;
		std::shared_ptr<OpenGLShader> m_pShader;
	};
}