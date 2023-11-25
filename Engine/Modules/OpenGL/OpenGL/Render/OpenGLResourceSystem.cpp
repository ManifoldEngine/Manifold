#include "OpenGLResourceSystem.h"

#include <OpenGL/Render/OpenGLBuffer.h>
#include <OpenGL/Render/OpenGLVertexArray.h>
#include <OpenGL/Render/OpenGLTexture.h>
#include <OpenGL/Render/OpenGLShader.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Texture.h>
#include <RenderAPI/Shader.h>

using namespace ECSEngine;

std::string_view OpenGLResourceSystem::getName() const
{
    return "OpenGLResourceSystem";
}

const std::shared_ptr<OpenGLVertexArray>& OpenGLResourceSystem::getVertexArray(AssetId assetId) 
{
	return m_vertexArrays[assetId];
}

const std::shared_ptr<OpenGLTexture2D>& OpenGLResourceSystem::getTexture(AssetId assetId) 
{
	return m_textures[assetId];
}

const std::shared_ptr<OpenGLShader>& OpenGLResourceSystem::getShader(AssetId assetId) 
{
	return m_shaders[assetId];
}

void OpenGLResourceSystem::onMeshLoaded(const std::shared_ptr<Mesh>& mesh)
{
	if (mesh == nullptr || mesh->vertices.empty() || mesh->indices.empty())
	{
		ECSE_LOG_ERROR(LogOpenGL, "Received a null or empty mesh");
		return;
	}

	std::vector<float> vertices;
	vertices.reserve((3 + 3 + 2) * mesh->vertices.size());

	size_t index = 0;
	for (const Vertex& vertex : mesh->vertices)
	{
		vertices.push_back(vertex.position.x);
		vertices.push_back(vertex.position.y);
		vertices.push_back(vertex.position.z);

		vertices.push_back(vertex.normal.x);
		vertices.push_back(vertex.normal.y);
		vertices.push_back(vertex.normal.z);

		vertices.push_back(vertex.textureCoordinate.x);
		vertices.push_back(vertex.textureCoordinate.y);
	}

	std::shared_ptr<OpenGLVertexBuffer> vertexBuffer = std::make_shared<OpenGLVertexBuffer>(&vertices[0], (int)(sizeof(float) * vertices.size()));
	vertexBuffer->layout =
	{
		{ ShaderDataType::Float3, false },
		{ ShaderDataType::Float3, true  },
		{ ShaderDataType::Float2, false }
	};

	std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(&mesh->indices[0], (int)sizeof(uint32_t) * mesh->indices.size());

	std::shared_ptr<OpenGLVertexArray> vertexArray = std::make_shared<OpenGLVertexArray>();
	vertexArray->addVertexBuffer(vertexBuffer);
	vertexArray->setIndexBuffer(indexBuffer);

	m_vertexArrays[mesh->id] = vertexArray;
}

void OpenGLResourceSystem::onTextureLoaded(const std::shared_ptr<Texture>& texture)
{
	if (texture == nullptr)
	{
		ECSE_LOG_ERROR(LogOpenGL, "Received a null texture");
		return;
	}

	m_textures[texture->id] = std::make_shared<OpenGLTexture2D>(texture->path);
}

void OpenGLResourceSystem::onShaderLoaded(const std::shared_ptr<Shader>& shaderAsset)
{
	if (shaderAsset == nullptr)
	{
		ECSE_LOG_ERROR(LogOpenGL, "Received null shader");
		return;
	}

	std::shared_ptr<OpenGLShader> shader = std::make_shared<OpenGLShader>(shaderAsset->name, shaderAsset->vertexSource, shaderAsset->fragmentSource);
	if (shader->compile())
	{
		m_shaders[shaderAsset->id] = shader;
	}
}
