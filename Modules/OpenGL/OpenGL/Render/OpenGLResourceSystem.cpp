#include "OpenGLResourceSystem.h"

#include <Assets/AssetSystem.h>

#include <OpenGL/Render/OpenGLBuffer.h>
#include <OpenGL/Render/OpenGLVertexArray.h>
#include <OpenGL/Render/OpenGLTexture.h>
#include <OpenGL/Render/OpenGLShader.h>
#include <OpenGL/Render/OpenGLMaterial.h>
#include <OpenGL/Render/OpenGLSprite.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/Sprite.h>

using namespace Mani;

std::string_view OpenGLResourceSystem::getName() const
{
    return "OpenGLResourceSystem";
}

void OpenGLResourceSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
	systemContainer.initializeDependency<AssetSystem>();

	AssetDatabase<Mesh>* meshDatabase = getOrAddDatabase<Mesh>(registry);
	AssetDatabase<Material>* materialDatabase = getOrAddDatabase<Material>(registry);
	AssetDatabase<Sprite>* spriteDatabase = getOrAddDatabase<Sprite>(registry);
	
	onMeshLoadedHandle		= meshDatabase->onAssetLoaded.subscribe(		std::bind_front(&OpenGLResourceSystem::onMeshLoaded, this));
	onMaterialLoadedHandle	= materialDatabase->onAssetLoaded.subscribe(	std::bind_front(&OpenGLResourceSystem::onMaterialLoaded, this));
	onSpriteLoadedHandle	= spriteDatabase->onAssetLoaded.subscribe(		std::bind_front(&OpenGLResourceSystem::onSpriteLoaded, this));
}

void OpenGLResourceSystem::onDeinitialize(ECS::Registry& registry)
{
	if (AssetDatabase<Mesh>* database = registry.getSingle<AssetDatabase<Mesh>>())
	{
		database->onAssetLoaded.unsubscribe(onMeshLoadedHandle);
	}

	if (AssetDatabase<Material>* database = registry.getSingle<AssetDatabase<Material>>())
	{
		database->onAssetLoaded.unsubscribe(onMaterialLoadedHandle);
	}

	if (AssetDatabase<Sprite>* database = registry.getSingle<AssetDatabase<Sprite>>())
	{
		database->onAssetLoaded.unsubscribe(onSpriteLoadedHandle);
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

const std::shared_ptr<OpenGLSprite>& Mani::OpenGLResourceSystem::getSprite(const std::string& name)
{
	return m_sprites[name];
}

void OpenGLResourceSystem::onMeshLoaded(ECS::Registry& registry, const std::weak_ptr<Mesh>& meshPtr)
{
	if (meshPtr.expired())
	{
		return;
	}

	std::shared_ptr<Mesh> mesh = meshPtr.lock();

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
		{ EShaderDataType::Float3, false },
		{ EShaderDataType::Float3, true  },
		{ EShaderDataType::Float2, false }
	};

	std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(&mesh->indices[0], (int)sizeof(uint32_t) * mesh->indices.size());

	std::shared_ptr<OpenGLVertexArray> vertexArray = std::make_shared<OpenGLVertexArray>();
	vertexArray->addVertexBuffer(vertexBuffer);
	vertexArray->setIndexBuffer(indexBuffer);

	m_vertexArrays[mesh->name] = vertexArray;
}

void OpenGLResourceSystem::onMaterialLoaded(ECS::Registry& registry, const std::weak_ptr<Material>& materialPtr)
{
	if (materialPtr.expired())
	{
		return;
	}

	std::shared_ptr<Material> material = materialPtr.lock();

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
		openGLMaterial->shader = getOrAddShaderName(registry, material->shaderPath);
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

void Mani::OpenGLResourceSystem::onSpriteLoaded(ECS::Registry& registry, const std::weak_ptr<Sprite>& spritePtr)
{
	if (spritePtr.expired())
	{
		return;
	}

	std::shared_ptr<Sprite> sprite = spritePtr.lock();

	if (sprite == nullptr)
	{
		MANI_LOG_ERROR(LogOpenGL, "Received a null sprite");
		return;
	}

	std::shared_ptr<OpenGLSprite> openGLSprite = std::make_shared<OpenGLSprite>();
	openGLSprite->name = sprite->name;

	if (!sprite->shaderPath.empty())
	{
		openGLSprite->shaderName = getOrAddShaderName(registry, sprite->shaderPath);
	}

	if (!sprite->texturePath.empty())
	{
		openGLSprite->textureName = getOrAddTextureName(sprite->texturePath);
	}

	m_sprites[openGLSprite->name] = openGLSprite;
}

const std::shared_ptr<OpenGLVertexArray>& Mani::OpenGLResourceSystem::getQuad(uint16_t repeatAmount)
{
	auto it = m_quadVertexArrays.find(repeatAmount);
	if (it != m_quadVertexArrays.end())
	{
		return it->second;
	}

	const float repeatAmountF = static_cast<float>(repeatAmount);

	// hardcoded 2d quad. We flip the X axis because OpenGL is right handed.
	std::vector<float> vertices =
	{
		//    vertex		//        texture
		0.0f, 0.0f, 1.0f,	0.0f,			repeatAmountF,
		-1.0f, 0.0f, 0.0f,	repeatAmountF,	0.0f,
		0.0f, 0.0f, 0.0f,	0.0f,			0.0f,
		0.0f, 0.0f, 1.0f,	0.0f,			repeatAmountF,
		-1.0f, 0.0f, 1.0f,	repeatAmountF,	repeatAmountF,
		-1.0f, 0.0f, 0.0f,	repeatAmountF,	0.0f,
	};

	std::shared_ptr<OpenGLVertexBuffer> vertexBuffer = std::make_shared<OpenGLVertexBuffer>(&vertices[0], (int)(sizeof(float) * vertices.size()));
	vertexBuffer->layout =
	{
		{ EShaderDataType::Float3, false },
		{ EShaderDataType::Float2, false }
	};

	m_quadVertexArrays[repeatAmount] = std::make_shared<OpenGLVertexArray>();
	const std::shared_ptr<OpenGLVertexArray>& vertexArray = m_quadVertexArrays[repeatAmount];
	vertexArray->addVertexBuffer(vertexBuffer);

	std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5 };
	std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(&indices[0], (int)sizeof(uint32_t) * indices.size());
	vertexArray->setIndexBuffer(indexBuffer);

	return vertexArray;
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

const std::string Mani::OpenGLResourceSystem::getOrAddShaderName(ECS::Registry& registry, const std::filesystem::path& path)
{
	const std::string shaderName = path.stem().string();
	if (!m_shaders.contains(shaderName))
	{
		// make sure we load the shader
		std::shared_ptr<Shader> shader = AssetSystem::loadAsset<Shader>(registry, path).lock();
		MANI_ASSERT(shader != nullptr, "Could not load shader");
		onShaderLoaded(shader);
	}

	return shaderName;
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
