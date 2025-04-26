#include "OpenGLResourceSystem.h"

#include <Resources/ResourceSystem.h>

#include <ECS/Entity.h>

#include <OpenGL/OpenGLWindowContext.h>
#include <OpenGL/Data/OpenGLBuffer.h>
#include <OpenGL/Data/OpenGLVertexArray.h>
#include <OpenGL/Data/OpenGLTexture.h>
#include <OpenGL/Data/OpenGLShader.h>
#include <OpenGL/Data/OpenGLMaterial.h>
#include <OpenGL/Data/OpenGLSprite.h>
#include <OpenGL/Data/STBITexture.h>

#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Sprite.h>
#include <RenderAPI/Shader.h>

#include <GLFW/glfw3.h>

using namespace Mani;

void OpenGLResourceSystemExtension::onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId) const
{
	const ECS::Entity* entity = registry.getEntity(entityId);
	if (entity == nullptr)
	{
		return;
	}

	const ECS::ComponentId meshId = registry.getComponentId<Resource<Mesh>>();
	if (entity->hasComponent(meshId))
	{
		OpenGLResourceSystem::onMeshLoaded(registry, entityId);
		return;
	}

	const ECS::ComponentId materialId = registry.getComponentId<Resource<Material>>();
	if (entity->hasComponent(materialId))
	{
		OpenGLResourceSystem::onMaterialLoaded(registry, entityId);
		return;
	}

	const ECS::ComponentId stbiTextureId = registry.getComponentId<Resource<STBITexture>>();
	if (entity->hasComponent(stbiTextureId))
	{
		OpenGLResourceSystem::onSTBITextureLoaded(registry, entityId);
		return;
	}

	const ECS::ComponentId spriteId = registry.getComponentId<Resource<Sprite>>();
	if (entity->hasComponent(spriteId))
	{
		OpenGLResourceSystem::onSpriteLoaded(registry, entityId);
		return;
	}

	const ECS::ComponentId shaderId = registry.getComponentId<Resource<Shader>>();
	if (entity->hasComponent(shaderId))
	{
		OpenGLResourceSystem::onShaderLoaded(registry, entityId);
		return;
	}
}

void OpenGLResourceSystemExtension::onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId) const
{
	const ECS::Entity* entity = registry.getEntity(entityId);
	if (entity == nullptr)
	{
		return;
	}

	const ECS::ComponentId meshId = registry.getComponentId<Resource<Mesh>>();
	if (entity->hasComponent(meshId))
	{
		OpenGLResourceSystem::onMeshUnloaded(registry, entityId);
		return;
	}

	const ECS::ComponentId materialId = registry.getComponentId<Resource<Material>>();
	if (entity->hasComponent(materialId))
	{
		OpenGLResourceSystem::onMaterialUnloaded(registry, entityId);
		return;
	}

	const ECS::ComponentId stbiTextureId = registry.getComponentId<Resource<STBITexture>>();
	if (entity->hasComponent(stbiTextureId))
	{
		OpenGLResourceSystem::onSTBITextureUnloaded(registry, entityId);
		return;
	}

	const ECS::ComponentId spriteId = registry.getComponentId<Resource<Sprite>>();
	if (entity->hasComponent(spriteId))
	{
		OpenGLResourceSystem::onSpriteUnloaded(registry, entityId);
		return;
	}
}

struct OpenGLResourceSystem::Storage
{
	ECS::EntityId extensionHandle = ECS::INVALID_ID;
};

void OpenGLResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ResourceSystem>();

	OpenGLResourceSystem::Storage& storage = *registry.addSingle<OpenGLResourceSystem::Storage>();
	auto ext = std::make_shared<OpenGLResourceSystemExtension>();
	storage.extensionHandle = ResourceSystem::addExtension(registry, ext);
}

void OpenGLResourceSystem::onDeinitialize(ECS::Registry& registry)
{
	OpenGLResourceSystem::Storage& storage = *registry.getSingle<OpenGLResourceSystem::Storage>();
	ResourceSystem::removeExtension(registry, storage.extensionHandle);

	registry.removeSingle<OpenGLResourceSystem::Storage>();
}

void OpenGLResourceSystem::onMeshLoaded(ECS::Registry& registry, ECS::EntityId meshId) 
{
	const Resource<Mesh>* meshRes = registry.get<Resource<Mesh>>(meshId);
	MANI_ASSERT(meshRes != nullptr && meshRes->isReady, "We expect the mesh to have been loaded");
	const Mesh& mesh = meshRes->value;
	
	// VAOs need to be created in the context they're going to be used in.
	OpenGLRenderSystem::enqueueRenderTask(registry, [&registry, &mesh, meshId] {
		OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>();
		MANI_ASSERT(context != nullptr, "Trying to load vao without a valid context");
		glfwMakeContextCurrent(context->window);

		Resource<OpenGLVertexArray>& res = *registry.add<Resource<OpenGLVertexArray>>(meshId);
		
		constexpr size_t vertexSize = 3 + 3 + 2;
		OpenGLVertexBuffer vertexBuffer = OpenGLVertexBuffer(&mesh.vertices[0].position.x, (int)(sizeof(float) * (mesh.vertices.size() * vertexSize)));;
		vertexBuffer.layout =
		{
			{ EShaderDataType::Float3, false },
			{ EShaderDataType::Float3, true  },
			{ EShaderDataType::Float2, false }
		};

		OpenGLIndexBuffer indexBuffer = OpenGLIndexBuffer(&mesh.indices[0], (int)sizeof(uint32_t) * mesh.indices.size());

		res.value.addVertexBuffer(std::move(vertexBuffer));
		res.value.setIndexBuffer(std::move(indexBuffer));

		res.isReady = true;
		glfwMakeContextCurrent(nullptr);
	});
}

void OpenGLResourceSystem::onMaterialLoaded(ECS::Registry& registry, ECS::EntityId materialId) 
{
	const Resource<Material>* materialRes = registry.get<Resource<Material>>(materialId);
	MANI_ASSERT(materialRes != nullptr && materialRes->isReady, "We expect the material to have been loaded");
	const Material& material = materialRes->value;

	Resource<OpenGLMaterial>& openglMaterialRes = *registry.add<Resource<OpenGLMaterial>>(materialId);
	OpenGLMaterial& openglMaterial = openglMaterialRes.value;

	openglMaterial.shaderId = ResourceSystem::loadResource<Shader>(registry, material.shaderPath);
	if (!material.diffusePath.empty())
	{
		openglMaterial.diffuseId = ResourceSystem::loadResource<STBITexture>(registry, material.diffusePath);
	}
	if (!material.specularPath.empty())
	{
		openglMaterial.specularId = ResourceSystem::loadResource<STBITexture>(registry, material.specularPath);
	}

	openglMaterial.name = material.name;
	openglMaterial.color = material.color;
	openglMaterial.shininess = material.shininess;

	openglMaterialRes.isReady = true;
}

void Mani::OpenGLResourceSystem::onShaderLoaded(ECS::Registry& registry, ECS::EntityId shaderId)
{
	Resource<Shader>* shaderRes = registry.get<Resource<Shader>>(shaderId);
	MANI_ASSERT(shaderRes != nullptr, "Shader loading flow should be synchronous");
	MANI_ASSERT(shaderRes->isReady, "Shader loading flow should be synchronous");

	const Shader& shader = shaderRes->value;
	Resource<OpenGLShader>& openGLShaderRes = *registry.add<Resource<OpenGLShader>>(shaderId);
	openGLShaderRes.value = OpenGLShader
	{
		shader.name,
		shader.vertexSource,
		shader.fragmentSource
	};

	// compile
	openGLShaderRes.value.compile();
	// if compiled, inject the shader in the resource system
	openGLShaderRes.isReady = openGLShaderRes.value.isCompiled();
}

void Mani::OpenGLResourceSystem::onSTBITextureLoaded(ECS::Registry& registry, ECS::EntityId entityId)
{
	Resource<STBITexture>* stbiTextureRes = registry.get<Resource<STBITexture>>(entityId);
	MANI_ASSERT(stbiTextureRes != nullptr && stbiTextureRes->isReady, "We expect the material to have been loaded");
	const STBITexture& stbiTexture = stbiTextureRes->value;

	Resource<OpenGLTexture2D>& textureRes = *registry.add<Resource<OpenGLTexture2D>>(entityId);
	textureRes.value = OpenGLTexture2D{ stbiTexture };
	textureRes.isReady = true;

	stbiTextureRes->value.~STBITexture();
}

void OpenGLResourceSystem::onSpriteLoaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onSpriteLoaded called"); }

void OpenGLResourceSystem::onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onMeshUnloaded called"); }
void OpenGLResourceSystem::onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onMaterialUnloaded called"); }
void OpenGLResourceSystem::onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onShaderUnloaded called"); }
void OpenGLResourceSystem::onSTBITextureUnloaded(ECS::Registry& registry, ECS::EntityId) { MANI_LOG(LogOpenGL, "onSTBITextureUnloaded called"); }
void OpenGLResourceSystem::onSpriteUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onSpriteUnloaded called"); }

template<>
bool ResourceLoader::load<STBITexture>(const std::filesystem::path& absolutePath, Resource<STBITexture>& resource)
{
	resource.value = STBITexture{ absolutePath.string() };
	return resource.value.data != nullptr;
}
