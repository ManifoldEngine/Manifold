#include "OpenGLResourceSystem.h"

#include <Resources/ResourceSystem.h>

#include <ECS/Entity.h>

#include <OpenGL/Data/OpenGLBuffer.h>
#include <OpenGL/Data/OpenGLVertexArray.h>
#include <OpenGL/Data/OpenGLTexture.h>
#include <OpenGL/Data/OpenGLShader.h>
#include <OpenGL/Data/OpenGLMaterial.h>
#include <OpenGL/Data/OpenGLSprite.h>
#include <OpenGL/Data/STBITexture.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Sprite.h>
#include <RenderAPI/Shader.h>

using namespace Mani;

void OpenGLResourceSystemExtension::onResourceCreated(ECS::Registry& registry, ECS::EntityId entityId) const
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

void OpenGLResourceSystemExtension::onResourceDestroyed(ECS::Registry& registry, ECS::EntityId entityId) const
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

	std::unordered_map<ECS::EntityId, ECS::EntityId> resourceMap = {};
	std::mutex resourceMutex;
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

ECS::EntityId OpenGLResourceSystem::getOpenGLResourceId(ECS::Registry& registry, ECS::EntityId entityId)
{
	// we could be accessing this while we're shutting down the application from the render thread
	if (OpenGLResourceSystem::Storage* storage = registry.getSingle<OpenGLResourceSystem::Storage>())
	{
		std::lock_guard<std::mutex> lock(storage->resourceMutex);
		auto it = storage->resourceMap.find(entityId);
		if (it == storage->resourceMap.end())
		{
			return ECS::INVALID_ID;
		}
		return it->second;
	}
	return ECS::INVALID_ID;
}

void OpenGLResourceSystem::onMeshLoaded(ECS::Registry& registry, ECS::EntityId meshId) 
{
	const ECS::EntityId openglResourceId = registry.create();
	registry.add<Resource<OpenGLVertexArray>>(openglResourceId);

	OpenGLResourceSystem::Storage& storage = *registry.getSingle<OpenGLResourceSystem::Storage>();
	{
		std::lock_guard<std::mutex> lock(storage.resourceMutex);
		storage.resourceMap[meshId] = openglResourceId;
	}
}

void OpenGLResourceSystem::onMaterialLoaded(ECS::Registry& registry, ECS::EntityId materialId) 
{
	const ECS::EntityId openglMaterialId = registry.create();
	Resource<OpenGLMaterial>* openglMaterialRes = registry.add<Resource<OpenGLMaterial>>(openglMaterialId);

	OpenGLResourceSystem::Storage& storage = *registry.getSingle<OpenGLResourceSystem::Storage>();
	{
		std::lock_guard<std::mutex> lock(storage.resourceMutex);
		storage.resourceMap[materialId] = openglMaterialId;
	}

	const ECS::EntityId diffuseId = registry.create();
	registry.add<Resource<STBITexture>>(diffuseId);
	
	const ECS::EntityId specularId = registry.create();
	registry.add<Resource<STBITexture>>(specularId);
	
	Resource<Material>* materialRes = registry.get<Resource<Material>>(materialId);
	Mani::enqueueTask([&registry, materialRes, openglMaterialRes, diffuseId, specularId]
	{
		while (!materialRes->isReady)
		{
			std::this_thread::yield();
		}

		const Material& material = materialRes->get();
		openglMaterialRes->value = std::make_unique<OpenGLMaterial>();
		openglMaterialRes->value->name = material.name;
		openglMaterialRes->value->color = material.color;
		openglMaterialRes->value->shininess = material.shininess;

		// shader
		const ECS::EntityId shaderId = ResourceSystem::loadResourceSync<Shader>(registry, material.shaderPath);
		openglMaterialRes->value->shaderId = OpenGLResourceSystem::getOpenGLResourceId(registry, shaderId);

		// diffuse
		Resource<STBITexture>* diffuse = registry.get<Resource<STBITexture>>(diffuseId);
		if (!material.diffusePath.empty())
		{
			diffuse->value = std::make_unique<STBITexture>(material.diffusePath);
			openglMaterialRes->value->diffuseId = diffuseId;
			diffuse->isReady = true;
		}
		else
		{
			registry.deferDestroy(diffuseId);
		}

		// specular
		Resource<STBITexture>* specular = registry.get<Resource<STBITexture>>(specularId);
		if (!material.specularPath.empty())
		{
			specular->value = std::make_unique<STBITexture>(material.specularPath);
			openglMaterialRes->value->specularId = specularId;
			specular->isReady = true;
		}
		else
		{
			registry.deferDestroy(specularId);
		}

		openglMaterialRes->isReady = true;
	});
}

void OpenGLResourceSystem::onSpriteLoaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onSpriteLoaded called"); }

void Mani::OpenGLResourceSystem::onShaderLoaded(ECS::Registry& registry, ECS::EntityId shaderId)
{
	Resource<Shader>* shaderRes = registry.get<Resource<Shader>>(shaderId);
	MANI_ASSERT(shaderRes != nullptr, "Shader loading flow should be synchronous");
	MANI_ASSERT(shaderRes->isReady, "Shader loading flow should be synchronous");

	const Shader& shader = shaderRes->get();
	// compile
	std::unique_ptr<OpenGLShader> openglShader = std::make_unique<OpenGLShader>(
		shader.name,
		shader.vertexSource,
		shader.fragmentSource
	);
	openglShader->compile();

	if (openglShader->isCompiled())
	{
		// if compiled, inject the shader in the resource system
		ECS::EntityId openGLShaderId = ResourceSystem::injectResource<OpenGLShader>(registry, std::move(openglShader));

		OpenGLResourceSystem::Storage& storage = *registry.getSingle<OpenGLResourceSystem::Storage>();
		{
			std::lock_guard<std::mutex> lock(storage.resourceMutex);
			storage.resourceMap[shaderId] = openGLShaderId;
		}
	}
}

void OpenGLResourceSystem::onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onMeshUnloaded called"); }
void OpenGLResourceSystem::onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onMaterialUnloaded called"); }
void OpenGLResourceSystem::onSpriteUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onSpriteUnloaded called"); }
void OpenGLResourceSystem::onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId) { MANI_LOG(LogOpenGL, "onShaderUnloaded called"); }
