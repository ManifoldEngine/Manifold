#include "OpenGLShaderSystem.h"
#include <OpenGL/Shader/OpenGLShader.h>
#include <OpenGL/Shader/ShaderUtils.h>
#include <Core/Log.h>
#include <Core/FileSystem.h>
#include <filesystem>

using namespace ECSEngine;

void OpenGLShaderSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	namespace fs = std::filesystem;

	fs::path rootPath;
	if (!FileSystem::tryGetRootPath(rootPath))
	{
		ECSE_LOG_ERROR(LogOpenGL, "Could not intialize OpenGLShaderSystem, could not get the root path");
		return;
	}

	for (const std::string& pathString : config.defaultShaderRelativePaths)
	{
		fs::path absolutePath = rootPath.concat(pathString);;
		std::vector<fs::path> filePaths = FileSystem::getAllFilesWithExtension(absolutePath, config.shaderExtension);
		for (const fs::path& filePath : filePaths)
		{
			loadShaderFromPath(filePath);
		}
	}
}

bool OpenGLShaderSystem::loadShaderFromPath(const std::filesystem::path& path)
{
	std::string fileName, vertexSource, fragmentSource;
	if (!parseShaderSourceFileFromPath(path, fileName, vertexSource, fragmentSource))
	{
		ECSE_LOG_ERROR(LogOpenGL, "Could not parse file at {}", path.string());
		return false;
	}
	
	return loadShaderFromSource(fileName, vertexSource, fragmentSource);
}

bool OpenGLShaderSystem::loadShaderFromSource(const std::string& name, const std::string_view& vertexSource, const std::string_view& fragmentSource)
{
	if (m_shaders.contains(name))
	{
		ECSE_LOG_ERROR(LogOpenGL, "OpenGLShader system already has a shader with name {}", name);
		return false;
	}

	std::shared_ptr<OpenGLShader> pShader = std::make_shared<OpenGLShader>(name, vertexSource, fragmentSource);
	if (pShader->compile())
	{
		m_shaders[pShader->name] = pShader;
		return true;
	}
	return false;
}

std::shared_ptr<OpenGLShader> OpenGLShaderSystem::getShader(const std::string_view& name) const
{
	if (auto it = m_shaders.find(name); it != m_shaders.end())
	{
		return it->second;
	}
	return nullptr;
}

