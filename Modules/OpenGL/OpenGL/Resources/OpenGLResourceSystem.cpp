#include "OpenGLResourceSystem.h"

#include <Resources/Resources.h>
#include <Resources/ResourceSystem.h>

#include <ECS/Entity.h>

#include <OpenGL/OpenGLWindowContext.h>
#include <OpenGL/Resources/OpenGLBuffer.h>
#include <OpenGL/Resources/OpenGLVertexArray.h>
#include <OpenGL/Resources/OpenGLTexture.h>
#include <OpenGL/Resources/OpenGLShader.h>
#include <OpenGL/Resources/OpenGLMaterial.h>

#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <OpenGL/STBI.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Shader.h>
#include <GLFW/glfw3.h>

using namespace Mani;

void OpenGLResourceSystemExtension::onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	const ECS::Entity* entity = registry.getEntity(entityId);
	if (entity == nullptr)
	{
		return;
	}

	const ECS::ComponentId meshId = registry.getComponentId<Resource<Mesh>>();
	if (entity->hasComponent(meshId))
	{
		OpenGLResourceSystem::onMeshLoaded(registry, entityId, tag);
		return;
	}

	const ECS::ComponentId materialId = registry.getComponentId<Resource<Material>>();
	if (entity->hasComponent(materialId))
	{
		OpenGLResourceSystem::onMaterialLoaded(registry, entityId, tag);
		return;
	}

	const ECS::ComponentId shaderId = registry.getComponentId<Resource<Shader>>();
	if (entity->hasComponent(shaderId))
	{
		OpenGLResourceSystem::onShaderLoaded(registry, entityId, tag);
		return;
	}

	const ECS::ComponentId textureId = registry.getComponentId<Resource<Texture>>();
	if (entity->hasComponent(textureId))
	{
		OpenGLResourceSystem::onTextureLoaded(registry, entityId, tag);
	}
}

void OpenGLResourceSystemExtension::onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	const ECS::Entity* entity = registry.getEntity(entityId);
	if (entity == nullptr)
	{
		return;
	}

	const ECS::ComponentId meshId = registry.getComponentId<Resource<Mesh>>();
	if (entity->hasComponent(meshId))
	{
		OpenGLResourceSystem::onMeshUnloaded(registry, entityId, tag);
	}

	const ECS::ComponentId materialId = registry.getComponentId<Resource<Material>>();
	if (entity->hasComponent(materialId))
	{
		OpenGLResourceSystem::onMaterialUnloaded(registry, entityId, tag);
	}

	const ECS::ComponentId texture2DId = registry.getComponentId<Resource<OpenGLTexture2D>>();
	if (entity->hasComponent(texture2DId))
	{
		OpenGLResourceSystem::onTexture2DUnloaded(registry, entityId, tag);
	}

	const ECS::ComponentId textureId = registry.getComponentId<Resource<Texture>>();
	if (entity->hasComponent(textureId))
	{
		OpenGLResourceSystem::onTextureUnloaded(registry, entityId, tag);
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
	Resources::registerExtension(registry, &resourceExtension);
	Resources::registerLoader(registry, &textureLoader);
}

void OpenGLResourceSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterLoader(registry, &textureLoader);
	Resources::unregisterExtension(registry, &resourceExtension);
	registry.removeSingle<OpenGLResourceSystem::Storage>();
}

void OpenGLResourceSystem::onMeshLoaded(ECS::Registry& registry, ECS::EntityId meshId, uint32_t tag) 
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
		OpenGLVertexBuffer vertexBuffer;
		vertexBuffer.create(&mesh.vertices[0].position.x, (int)(sizeof(float) * (mesh.vertices.count() * vertexSize)));;
		vertexBuffer.layout =
		{
			{ EShaderDataType::Float3, false },
			{ EShaderDataType::Float3, true  },
			{ EShaderDataType::Float2, false }
		};

		OpenGLIndexBuffer indexBuffer;
		indexBuffer.create(&mesh.indices[0], (int)sizeof(uint32_t) * mesh.indices.count());

		res.value.create();
		res.value.addVertexBuffer(std::move(vertexBuffer));
		res.value.setIndexBuffer(std::move(indexBuffer));

		res.isReady = true;
		glfwMakeContextCurrent(nullptr);
	});
}

void OpenGLResourceSystem::onMaterialLoaded(ECS::Registry& registry, ECS::EntityId materialId, uint32_t tag)
{
	const Resource<Material>* materialRes = registry.get<Resource<Material>>(materialId);
	MANI_ASSERT(materialRes != nullptr && materialRes->isReady, "We expect the material to have been loaded");
	const Material& material = materialRes->value;

	Resource<OpenGLMaterial>& openglMaterialRes = *registry.add<Resource<OpenGLMaterial>>(materialId);
	OpenGLMaterial& openglMaterial = openglMaterialRes.value;

	openglMaterial.shaderId = Resources::load<Shader>(registry, material.shaderPath, tag);
	for (const ShaderParam_Texture& texture : material.textures)
	{
		openglMaterial.textures.add({
			.key = texture.key,
			.id = Resources::load<Texture>(registry, texture.path, tag),
		});
	}
	openglMaterial.name = material.name;
	List<OpenGLMaterial::ShaderParam>& shaderParameters = openglMaterial.shaderParameters;

	auto fillShaderParameters = [&shaderParameters]<typename T>(const List<T>& customParams)
	{
		for (const T& param : customParams)
		{
			shaderParameters.add({ param.key, param.value });
		}
	};

	fillShaderParameters(material.shaderParam_float_values);
	fillShaderParameters(material.shaderParam_Vec2f_values);
	fillShaderParameters(material.shaderParam_Vec3f_values);
	fillShaderParameters(material.shaderParam_Vec4f_values);
	fillShaderParameters(material.shaderParam_int_values);
	fillShaderParameters(material.shaderParam_Vec2i_values);
	fillShaderParameters(material.shaderParam_Vec3i_values);
	fillShaderParameters(material.shaderParam_Vec4i_values);
	fillShaderParameters(material.shaderParam_Mat3f_values);
	fillShaderParameters(material.shaderParam_Mat4f_values);
	fillShaderParameters(material.shaderParam_bool_values);

	openglMaterialRes.isReady = true;
}

void Mani::OpenGLResourceSystem::onShaderLoaded(ECS::Registry& registry, ECS::EntityId shaderId, uint32_t tag)
{
	Resource<Shader>* shaderRes = registry.get<Resource<Shader>>(shaderId);
	MANI_ASSERT(shaderRes != nullptr, "Shader loading flow should be synchronous");
	MANI_ASSERT(shaderRes->isReady, "Shader loading flow should be synchronous");

	const Shader& shader = shaderRes->value;
	Resource<OpenGLShader>& openGLShaderRes = *registry.add<Resource<OpenGLShader>>(shaderId);

	openGLShaderRes.value.create(shader.name, shader.vertexSource, shader.fragmentSource);
	openGLShaderRes.value.compile();
	// if compiled, inject the shader in the resource system
	openGLShaderRes.isReady = openGLShaderRes.value.isCompiled();
}

void Mani::OpenGLResourceSystem::onTextureLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	Resource<Texture>* textureRes = registry.get<Resource<Texture>>(entityId);
	MANI_ASSERT(textureRes != nullptr && textureRes->isReady, "We expect the material to have been loaded");
	Texture& texture = textureRes->value;

	OpenGLRenderSystem::enqueueRenderTask(registry, [&registry, &texture, entityId] {
		OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>();
		MANI_ASSERT(context != nullptr, "Trying to load vao without a valid context");
		glfwMakeContextCurrent(context->window);

		Resource<OpenGLTexture2D>& textureRes = *registry.add<Resource<OpenGLTexture2D>>(entityId);

		if (!textureRes.value.load(texture))
		{
			MANI_LOG_ERROR(LogOpenGL, "Failed to load texture with id {}", entityId);
		}
		textureRes.isReady = true;
		STBI::freeTexture(texture);
		glfwMakeContextCurrent(nullptr);
	});
}

constexpr std::string_view UNLOAD_BEFORE_READY_ERROR_MESSAGE = "unloading a resource before it is ready, this is unsupported";

void OpenGLResourceSystem::onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{ 
	Resource<OpenGLVertexArray>* res = registry.get<Resource<OpenGLVertexArray>>(entityId);
	if (res == nullptr)
	{
		return;
	}
	MANI_ASSERT(res->isReady, UNLOAD_BEFORE_READY_ERROR_MESSAGE);

	res->value.destroy();
	res->isReady = false;
}

void OpenGLResourceSystem::onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) 
{ 
	Resource<OpenGLMaterial>* res = registry.get<Resource<OpenGLMaterial>>(entityId);
	if (res == nullptr)
	{
		return;
	}
	MANI_ASSERT(res->isReady, UNLOAD_BEFORE_READY_ERROR_MESSAGE);

	for (const auto& [key, texture] : res->value.textures)
	{
		Resources::unload(registry, texture);
	}
}

void OpenGLResourceSystem::onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) 
{ 
	Resource<OpenGLShader>* res = registry.get<Resource<OpenGLShader>>(entityId);
	if (res == nullptr)
	{
		return;
	}
	MANI_ASSERT(res->isReady, UNLOAD_BEFORE_READY_ERROR_MESSAGE);
	res->value.destroy();
	res->isReady = false;
}

void Mani::OpenGLResourceSystem::onTextureUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	Resource<Texture>* res = registry.get<Resource<Texture>>(entityId);
	if (res == nullptr)
	{
		return;
	}
	MANI_ASSERT(res->isReady, UNLOAD_BEFORE_READY_ERROR_MESSAGE);
	if (STBI::isLoaded(res->value))
	{
		STBI::freeTexture(res->value);
	}
	res->isReady = false;
}

void Mani::OpenGLResourceSystem::onTexture2DUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	Resource<OpenGLTexture2D>* res = registry.get<Resource<OpenGLTexture2D>>(entityId);
	if (res == nullptr)
	{
		return;
	}
	MANI_ASSERT(res->isReady, UNLOAD_BEFORE_READY_ERROR_MESSAGE);
	res->value.unload();
	res->isReady = false;
}
