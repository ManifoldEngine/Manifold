#include "OpenGLResourceSystem.h"

#include <Assets/AssetSystem.h>

#include <OpenGL/Render/OpenGLBuffer.h>
#include <OpenGL/Render/OpenGLVertexArray.h>
#include <OpenGL/Render/OpenGLTexture.h>
#include <OpenGL/Render/OpenGLShader.h>
#include <OpenGL/Render/OpenGLMaterial.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Shader.h>

using namespace Mani;

std::string_view OpenGLResourceSystem::getName() const
{
    return "OpenGLResourceSystem";
}

void OpenGLResourceSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
	if (!m_assetSystem.expired())
	{
		std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();
		onAssetLoadedHandle = assetSystem->onJsonAssetLoaded.subscribe(std::bind(&OpenGLResourceSystem::onJsonAssetLoaded, this, std::placeholders::_1));
	}
}

void OpenGLResourceSystem::onDeinitialize(EntityRegistry& registry)
{
	if (!m_assetSystem.expired())
	{
		std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();
		assetSystem->onJsonAssetLoaded.unsubscribe(onAssetLoadedHandle);
	}
}

const std::shared_ptr<OpenGLVertexArray>& OpenGLResourceSystem::getVertexArray(const std::string& name) 
{
	return m_vertexArrays[name];
}

const std::shared_ptr<OpenGLTexture2D>& OpenGLResourceSystem::getTexture(const std::string& name)
{
	return m_textures[name];
}

const std::shared_ptr<OpenGLMaterial>& OpenGLResourceSystem::getMaterial(const std::string& name)
{
	return m_materials[name];
}

const std::shared_ptr<OpenGLShader>& OpenGLResourceSystem::getShader(const std::string& name)
{
	return m_shaders[name];
}

void OpenGLResourceSystem::onMeshLoaded(const std::shared_ptr<Mesh>& mesh)
{
	if (mesh == nullptr || mesh->vertices.empty() || mesh->indices.empty())
	{
		MANI_LOG_ERROR(LogOpenGL, "Received a null or empty mesh");
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

	m_vertexArrays[mesh->name] = vertexArray;
}

void OpenGLResourceSystem::onMaterialLoaded(const std::shared_ptr<Material>& material)
{
	if (material == nullptr)
	{
		MANI_LOG_ERROR(LogOpenGL, "Received a null material");
		return;
	}
	std::shared_ptr<OpenGLMaterial> openGLMaterial = std::make_shared<OpenGLMaterial>();
	openGLMaterial->name = material->name;
	openGLMaterial->color = material->color;
	openGLMaterial->shininess = material->shininess;

	// shader
	if (!material->shaderPath.empty())
	{
		const std::string shaderName = material->shaderPath.stem().string();
		openGLMaterial->shader = shaderName;

		if (!m_shaders.contains(shaderName) && !m_assetSystem.expired())
		{
			// make sure we load the shader
			std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();
			std::shared_ptr<Shader> shader = assetSystem->loadJsonAsset<Shader>(material->shaderPath);
			onShaderLoaded(shader);
		}
	}

	// diffuse
	if (!material->diffusePath.empty())
	{
		openGLMaterial->diffuse = getOrAddTextureName(material->diffusePath);
	}

	// specular
	if (!material->specularPath.empty())
	{
		openGLMaterial->diffuse = getOrAddTextureName(material->specularPath);
	}

	m_materials[openGLMaterial->name] = openGLMaterial;
}

const std::string OpenGLResourceSystem::getOrAddTextureName(const std::filesystem::path& originalPath)
{
	const std::string textureName = originalPath.filename().string();
	if (!m_textures.contains(textureName))
	{
		std::filesystem::path path = originalPath;
		if (path.is_relative())
		{
			std::filesystem::path rootPath;
			MANI_ASSERT(FileSystem::tryGetRootPath(rootPath), "we should be able to recover root path at this point.");
			path = rootPath.append(path.string());
		}
		m_textures[textureName] = std::make_shared<OpenGLTexture2D>(path.string());
	}

	return textureName;
}

void OpenGLResourceSystem::onShaderLoaded(const std::shared_ptr<Shader>& shaderAsset)
{
	if (shaderAsset == nullptr)
	{
		MANI_LOG_ERROR(LogOpenGL, "Received null shader");
		return;
	}

	std::shared_ptr<OpenGLShader> shader = std::make_shared<OpenGLShader>(shaderAsset->name, shaderAsset->vertexSource, shaderAsset->fragmentSource);
	if (shader->compile())
	{
		m_shaders[shaderAsset->name] = shader;
	}
}

void OpenGLResourceSystem::onJsonAssetLoaded(const std::shared_ptr<IJsonAsset>& jsonAsset)
{
	if (const auto& mesh = std::dynamic_pointer_cast<Mesh>(jsonAsset))
	{
		onMeshLoaded(mesh);
	}
	else if (const auto& material = std::dynamic_pointer_cast<Material>(jsonAsset))
	{
		onMaterialLoaded(material);
	}
}
