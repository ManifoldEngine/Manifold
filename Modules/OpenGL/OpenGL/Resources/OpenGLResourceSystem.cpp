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


void onMeshLoaded(ECS::Registry& registry, ECS::EntityId meshId, uint32_t tag)
{
	registry.addPinned<Resource<OpenGLVertexArray>>(meshId);

	// VAOs need to be created in the context they're going to be used in.
	OpenGL::enqueueRenderTask(registry, [&registry, meshId] {
		OpenGLWindowContext* context = registry.findSinglePinned<OpenGLWindowContext>();
		MANI_ASSERT(context != nullptr, "Trying to load vao without a valid context");
		glfwMakeContextCurrent(context->window);

		Resource<Mesh>& mesh = registry.getPinned<Resource<Mesh>>(meshId);
		Resource<OpenGLVertexArray>& vao = registry.getPinned<Resource<OpenGLVertexArray>>(meshId);

		constexpr size_t vertexSize = 3 + 3 + 2;
		OpenGLVertexBuffer vertexBuffer;
		vertexBuffer.create(&mesh.value.vertices[0].position.x, (int)(sizeof(float) * (mesh.value.vertices.count() * vertexSize)));;
		vertexBuffer.layout =
		{
			{ EShaderDataType::Float3, false },
			{ EShaderDataType::Float3, true  },
			{ EShaderDataType::Float2, false }
		};

		OpenGLIndexBuffer indexBuffer;
		indexBuffer.create(&mesh.value.indices[0], (int)sizeof(uint32_t) * mesh.value.indices.count());

		vao.value.create();
		vao.value.addVertexBuffer(std::move(vertexBuffer));
		vao.value.setIndexBuffer(std::move(indexBuffer));

		glfwMakeContextCurrent(nullptr);
	});
}

void onMaterialLoaded(ECS::Registry& registry, ECS::EntityId materialId, uint32_t tag)
{
	Resource<Material>& materialRes = registry.getPinned<Resource<Material>>(materialId);
	Resource<OpenGLMaterial>& openglMaterialRes = registry.addPinned<Resource<OpenGLMaterial>>(materialId);

	openglMaterialRes.value.shaderId = Resources::load<Shader>(registry, materialRes.value.shaderPath, tag);
	for (const ShaderParam_Texture& texture : materialRes.value.textures)
	{
		openglMaterialRes.value.textures.add({
			.key = texture.key,
			.id = Resources::load<Texture>(registry, texture.path, tag),
		});
	}
	openglMaterialRes.value.name = materialRes.value.name;
	List<OpenGLMaterial::ShaderParam>& shaderParameters = openglMaterialRes.value.shaderParameters;

	auto fillShaderParameters = [&shaderParameters]<typename T>(const List<T>&customParams)
	{
		for (const T& param : customParams)
		{
			shaderParameters.add({ param.key, param.value });
		}
	};

	{
		const Material& material = materialRes.value;
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
	}
}

void onShaderLoaded(ECS::Registry& registry, ECS::EntityId shaderId, uint32_t tag)
{
	Resource<OpenGLShader>& openGLShaderRes = registry.addPinned<Resource<OpenGLShader>>(shaderId);
	const Shader& shader = registry.getPinned<Resource<Shader>>(shaderId).value;

	openGLShaderRes.value.create(shader.name, shader.vertexSource, shader.fragmentSource);
	OpenGL::enqueueRenderTask(registry, [&registry, shaderId] 
	{
		OpenGLWindowContext* context = registry.findSinglePinned<OpenGLWindowContext>();
		MANI_ASSERT(context != nullptr, "Trying to compile shader without a valid opengl context");
		glfwMakeContextCurrent(context->window);
		Resource<OpenGLShader>& res = registry.getPinned<Resource<OpenGLShader>>(shaderId);

		res.value.compile();

		glfwMakeContextCurrent(nullptr);
	});
}

void onTextureLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	registry.addPinned<Resource<OpenGLTexture2D>>(entityId);

	OpenGL::enqueueRenderTask(registry, [&registry, entityId] 
	{
		OpenGLWindowContext* context = registry.findSinglePinned<OpenGLWindowContext>();
		MANI_ASSERT(context != nullptr, "Trying to load vao without a valid context");
		glfwMakeContextCurrent(context->window);

		Resource<Texture>& textureRes = registry.getPinned<Resource<Texture>>(entityId);
		Texture& texture = textureRes.value;

		Resource<OpenGLTexture2D>& openglTextureRes = registry.getPinned<Resource<OpenGLTexture2D>>(entityId);

		if (!openglTextureRes.value.load(texture))
		{
			MANI_LOG_ERROR(LogOpenGL, "Failed to load texture with id {}", entityId);
		}

		STBI::freeTexture(texture);
		glfwMakeContextCurrent(nullptr);
	});
}

constexpr std::string_view UNLOAD_BEFORE_READY_ERROR_MESSAGE = "unloading a resource before it is ready, this is unsupported";

void onMeshUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	if (Resource<OpenGLVertexArray>* res = registry.findPinned<Resource<OpenGLVertexArray>>(entityId))
	{
		MANI_ASSERT(Resources::isReady(registry, entityId), UNLOAD_BEFORE_READY_ERROR_MESSAGE);
		res->value.destroy();
	}
}

void onMaterialUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	if (Resource<OpenGLMaterial>* res = registry.findPinned<Resource<OpenGLMaterial>>(entityId))
	{
		MANI_ASSERT(Resources::isReady(registry, entityId), UNLOAD_BEFORE_READY_ERROR_MESSAGE);

		for (const auto& [key, texture] : res->value.textures)
		{
			Resources::unload(registry, texture);
		}
	}
}

void onShaderUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	if (Resource<OpenGLShader>* res = registry.findPinned<Resource<OpenGLShader>>(entityId))
	{
		MANI_ASSERT(Resources::isReady(registry, entityId), UNLOAD_BEFORE_READY_ERROR_MESSAGE);
		res->value.destroy();
	}
}

void onTextureUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	if (Resource<Texture>* res = registry.findPinned<Resource<Texture>>(entityId))
	{
		MANI_ASSERT(Resources::isReady(registry, entityId), UNLOAD_BEFORE_READY_ERROR_MESSAGE);
		if (STBI::isLoaded(res->value))
		{
			STBI::freeTexture(res->value);
		}
	}
}

void onTexture2DUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag)
{
	if (Resource<OpenGLTexture2D>* res = registry.findPinned<Resource<OpenGLTexture2D>>(entityId))
	{
		MANI_ASSERT(Resources::isReady(registry, entityId), UNLOAD_BEFORE_READY_ERROR_MESSAGE);
		res->value.unload();
	}
}

void OpenGLResourceSystemExtension::onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	if (registry.hasPinned<Resource<Mesh>>(entityId))
	{
		onMeshLoaded(registry, entityId, tag);
		return;
	}

	if (registry.hasPinned<Resource<Material>>(entityId))
	{
		onMaterialLoaded(registry, entityId, tag);
		return;
	}

	if (registry.hasPinned<Resource<Shader>>(entityId))
	{
		onShaderLoaded(registry, entityId, tag);
		return;
	}

	if (registry.hasPinned<Resource<Texture>>(entityId))
	{
		onTextureLoaded(registry, entityId, tag);
		return;
	}
}

void OpenGLResourceSystemExtension::onResourceUnloaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t tag) const
{
	if (registry.hasPinned<Resource<Mesh>>(entityId))
	{
		onMeshUnloaded(registry, entityId, tag);
	}

	if (registry.hasPinned<Resource<Material>>(entityId))
	{
		onMaterialUnloaded(registry, entityId, tag);
	}

	if (registry.hasPinned<Resource<OpenGLTexture2D>>(entityId))
	{
		onTexture2DUnloaded(registry, entityId, tag);
	}

	if (registry.hasPinned<Resource<Texture>>(entityId))
	{
		onTextureUnloaded(registry, entityId, tag);
	}
}

void OpenGLResourceSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<ResourceSystem>();

	Resources::registerExtension(registry, &m_resourceExtension);
	Resources::registerLoaderFor<Texture>(registry, &m_textureLoader);
}

void OpenGLResourceSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	Resources::unregisterLoaderFor<Texture>(registry);
	Resources::unregisterExtension(registry, &m_resourceExtension);
}

