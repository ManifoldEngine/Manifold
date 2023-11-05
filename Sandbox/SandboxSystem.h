#pragma once

#include <Core/System/System.h>
#include <OpenGL/OpenGLVertexArray.h>
#include <OpenGL/OpenGLBuffer.h>
#include <OpenGL/Shader/OpenGLShader.h>
#include <memory>
#include <OpenGL/OpenGLTexture.h>

namespace ECSEngine
{
	class EntityRegistry;
}

class SandboxSystem : public ECSEngine::SystemBase
{
public:
	virtual void tick(float deltaTime, ECSEngine::EntityRegistry& registry) override;

protected:
	virtual bool shouldTick(ECSEngine::EntityRegistry& registry) const override { return true; }
	virtual std::string_view getName() const override { return "SandboxSystem"; }

	virtual void onInitialize(ECSEngine::EntityRegistry& registry, ECSEngine::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(ECSEngine::EntityRegistry& registry) override;

private:
	std::unique_ptr<ECSEngine::OpenGLVertexArray> m_squareVertexArray;
	std::unique_ptr<ECSEngine::OpenGLIndexBuffer> m_indexBuffer;
	std::shared_ptr<ECSEngine::OpenGLShader> m_shader;
	std::unique_ptr<ECSEngine::OpenGLTexture2D> m_woodenBoxTexture2D;
	std::unique_ptr<ECSEngine::OpenGLTexture2D> m_awesomeFaceTexture2D;
};

