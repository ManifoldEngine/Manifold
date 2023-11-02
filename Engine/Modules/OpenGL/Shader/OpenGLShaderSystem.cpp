#include "OpenGLShaderSystem.h"
#include <OpenGL/Shader/OpenGLShader.h>
#include <OpenGL/Shader/ShaderUtils.h>
#include <Core/Log.h>

using namespace ECSEngine;

bool OpenGLShaderSystem::loadShaderFromPath(const std::string& path)
{
	std::string fileName, vertexSource, fragmentSource;
	if (!parseShaderSourceFileFromPath(path, fileName, vertexSource, fragmentSource))
	{
		ECSE_LOG_ERROR(LogOpenGL, "Could not parse file at {}", path);
		return false;
	}
	
	return loadShaderFromSource(fileName, vertexSource, fragmentSource);
}

bool OpenGLShaderSystem::loadShaderFromSource(const std::string_view& name, const std::string_view& vertexSource, const std::string_view& fragmentSource)
{
	if (m_shaders.contains(name))
	{
		ECSE_LOG_ERROR(LogOpenGL, "OpenGLShader system already has a shader with name {}", name);
		return false;
	}

	std::shared_ptr<OpenGLShader> pShader = std::make_shared<OpenGLShader>(name, vertexSource, fragmentSource);
	if (pShader->compile())
	{
		m_shaders[name] = pShader;
		return true;
	}
	return false;
}

std::shared_ptr<const OpenGLShader> ECSEngine::OpenGLShaderSystem::getShader(const std::string_view& name) const
{
	if (auto it = m_shaders.find(name); it != m_shaders.end())
	{
		return it->second;
	}
	return nullptr;
}

