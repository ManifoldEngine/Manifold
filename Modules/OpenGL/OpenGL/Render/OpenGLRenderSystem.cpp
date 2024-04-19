#include "OpenGLRenderSystem.h"
#include <Core/Log.h>
#include <Core/ManiAssert.h>

#include <Core/Components/Transform.h>

#include <ECS/EntityRegistry.h>
#include <ECS/RegistryView.h>

#include <Camera/CameraSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/Light/DirectionalLightComponent.h>
#include <RenderAPI/Light/PointLightComponent.h>
#include <RenderAPI/Light/SpotlightComponent.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/OpenGLSystem.h>
#include <OpenGL/Render/OpenGLResourceSystem.h>
#include <OpenGL/Render/OpenGLBuffer.h>
#include <OpenGL/Render/OpenGLVertexArray.h>
#include <OpenGL/Render/OpenGLTexture.h>
#include <OpenGL/Render/OpenGLMaterial.h>
#include <OpenGL/Render/OpenGLShader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

using namespace Mani;

std::string_view OpenGLRenderSystem::getName() const
{
	return "OpenGLRenderSystem";
}

bool OpenGLRenderSystem::shouldTick(EntityRegistry& registry) const
{
	return true;
}

void OpenGLRenderSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_resourceSystem = systemContainer.initializeDependency<OpenGLResourceSystem>();
	m_cameraSystem = systemContainer.initializeDependency<CameraSystem>();
}

void OpenGLRenderSystem::tick(float deltaTime, EntityRegistry& registry)
{
	if (m_resourceSystem.expired() || m_cameraSystem.expired())
	{
		return;
	}

	uint32_t x, y, width, height;
	getViewport(x, y, width, height);
	RegistryView<CameraComponent> cameraView(registry);
	for (const auto& entityId : cameraView)
	{
		CameraComponent* cameraComponent = registry.getComponent<CameraComponent>(entityId);
		cameraComponent->config.width = static_cast<float>(width);
		cameraComponent->config.height = static_cast<float>(height);
	}

	std::shared_ptr<OpenGLResourceSystem> resourceSystem = m_resourceSystem.lock();
	
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec3 cameraPosition;
	{
		std::shared_ptr<CameraSystem> cameraSystem = m_cameraSystem.lock();
		const Transform* cameraTransform = cameraSystem->getCameraTransform(registry);
		if (cameraTransform == nullptr)
		{
			return;
		}

		cameraPosition = cameraTransform->position;

		const CameraComponent* cameraComponent = cameraSystem->getCameraComponent(registry);
		if (cameraComponent == nullptr)
		{
			return;
		}

		viewMatrix = cameraComponent->view;
		projectionMatrix = cameraComponent->projection;
	}

	glEnable(GL_DEPTH_TEST);
	
	// setting color state.
	glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);

	// consuming color state.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RegistryView<DirectionalLightComponent> directionalLightsView(registry);
	RegistryView<Transform, PointLightComponent> pointLightsView(registry);
	RegistryView<Transform, SpotlightComponent> spotlightsView(registry);

	RegistryView<Transform, MeshComponent> meshesView(registry);

	for (const EntityId entityId : meshesView)
	{
		Transform* transform = registry.getComponent<Transform>(entityId);
		const MeshComponent* meshComponent = registry.getComponent<MeshComponent>(entityId);

		if (meshComponent->mesh == nullptr)
		{
			MANI_LOG_WARNING(LogOpenGL, "Entity {} with null mesh or material", entityId);
			continue;
		}

		const std::shared_ptr<Mesh>& mesh = meshComponent->mesh;
		const std::shared_ptr<Material>& materialAsset = meshComponent->material;

		if (mesh == nullptr || materialAsset == nullptr)
		{
			MANI_LOG_WARNING(LogOpenGL, "Entity {} with null mesh or material", entityId);
			continue;
		}

		const std::shared_ptr<OpenGLVertexArray>& vao = resourceSystem->getVertexArray(mesh->name);
		if (vao == nullptr)
		{
			MANI_LOG_WARNING(LogOpenGL, "Attempting to draw a mesh that is not loaded");
			continue;
		}

		const std::shared_ptr<OpenGLMaterial> material = resourceSystem->getMaterial(materialAsset->name);
		if (material == nullptr)
		{
			MANI_LOG_WARNING(LogOpenGL, "Attempting to draw a material that is not loaded");
			continue;
		}

		MANI_ASSERT(!material->shader.empty(), "A Shader is always expected in a material");

		std::shared_ptr<OpenGLShader> shader = resourceSystem->getShader(material->shader);
		if (shader == nullptr)
		{
			MANI_LOG_WARNING(LogOpenGL, "Attempting to draw a mesh with an uncompiled shader");
			continue;
		}

		shader->use();

		glm::mat4 modelMatrix = transform->calculateModelMatrix();
		glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));

		// set vertex uniforms
		shader->setFloatMatrix4("model", glm::value_ptr(modelMatrix));
		shader->setFloatMatrix3("normalMatrix", glm::value_ptr(normalMatrix));
		shader->setFloatMatrix4("view", glm::value_ptr(viewMatrix));
		shader->setFloatMatrix4("projection", glm::value_ptr(projectionMatrix));

		// set fragment uniforms
		shader->setFloat3("viewPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);

		int textureIndex = 0;

		// set material
		std::shared_ptr<OpenGLTexture2D> diffuseTexture = nullptr;
		if (!material->diffuse.empty())
		{
			diffuseTexture = resourceSystem->getTexture(material->diffuse);
			if (diffuseTexture != nullptr)
			{
				diffuseTexture->bind(textureIndex);
				shader->setTextureSlot("material.diffuseMap", textureIndex++);
			}
		}

		std::shared_ptr<OpenGLTexture2D> specularTexture = nullptr;
		if (!material->specular.empty())
		{
			specularTexture = resourceSystem->getTexture(material->specular);
			if (specularTexture != nullptr)
			{
				specularTexture->bind(textureIndex);
				shader->setTextureSlot("material.specularMap", textureIndex++);
			}
		}

		shader->setFloat("material.shininess", material->shininess);

		// set lights
		int directionalLightIndex = 0;
		int pointLightIndex = 0;
		int spotlightIndex = 0;
		for(const EntityId entityId : directionalLightsView)
		{
			const DirectionalLightComponent* light = registry.getComponent<DirectionalLightComponent>(entityId);

			const std::string directionalLightArray = std::format("directionalLights[{}]", directionalLightIndex);
			shader->setFloat3(std::format("{}.direction", directionalLightArray).c_str(), light->direction.x, light->direction.y, light->direction.z);
			shader->setFloat3(std::format("{}.ambient", directionalLightArray).c_str(), light->ambient.x, light->ambient.y, light->ambient.z);
			shader->setFloat3(std::format("{}.diffuse", directionalLightArray).c_str(), light->diffuse.x, light->diffuse.y, light->diffuse.z);
			shader->setFloat3(std::format("{}.specular", directionalLightArray).c_str(), light->specular.x, light->specular.x, light->specular.x);
			directionalLightIndex++;
		}

		for (const EntityId entityId : pointLightsView)
		{
			const Transform* transform = registry.getComponent<Transform>(entityId);
			const PointLightComponent* light = registry.getComponent<PointLightComponent>(entityId);

			const std::string pointLightArray = std::format("pointLights[{}]", pointLightIndex);
			shader->setFloat3(std::format("{}.position", pointLightArray).c_str(), transform->position.x, transform->position.y, transform->position.z);

			shader->setFloat3(std::format("{}.ambient", pointLightArray).c_str(), light->ambient.x, light->ambient.y, light->ambient.z);
			shader->setFloat3(std::format("{}.diffuse", pointLightArray).c_str(), light->diffuse.x, light->diffuse.y, light->diffuse.z);
			shader->setFloat3(std::format("{}.specular", pointLightArray).c_str(), light->specular.x, light->specular.x, light->specular.x);

			shader->setFloat(std::format("{}.constant", pointLightArray).c_str(), light->constant);
			shader->setFloat(std::format("{}.linear", pointLightArray).c_str(), light->linear);
			shader->setFloat(std::format("{}.quadratic", pointLightArray).c_str(), light->quadratic);

			pointLightIndex++;
		}

		for (const EntityId entityId : spotlightsView)
		{
			const Transform* transform = registry.getComponent<Transform>(entityId);
			const SpotlightComponent* light = registry.getComponent<SpotlightComponent>(entityId);
			const glm::vec3 forward = transform->forward();

			const std::string spotlightsArray = std::format("spotlights[{}]", spotlightIndex);
			shader->setFloat3(std::format("{}.position", spotlightsArray).c_str(), transform->position.x, transform->position.y, transform->position.z);
			shader->setFloat3(std::format("{}.direction", spotlightsArray).c_str(), forward.x, forward.y, forward.z);
			shader->setFloat(std::format("{}.cutOff", spotlightsArray).c_str(), light->cutOff);
			shader->setFloat(std::format("{}.outterCutOff", spotlightsArray).c_str(), light->outterCutOff);

			shader->setFloat3(std::format("{}.ambient", spotlightsArray).c_str(), light->ambient.x, light->ambient.y, light->ambient.z);
			shader->setFloat3(std::format("{}.diffuse", spotlightsArray).c_str(), light->diffuse.x, light->diffuse.y, light->diffuse.z);
			shader->setFloat3(std::format("{}.specular", spotlightsArray).c_str(), light->specular.x, light->specular.x, light->specular.x);

			spotlightIndex++;
		}

		shader->setInt("directionalLightsCount", directionalLightIndex);
		shader->setInt("pointLightsCount", pointLightIndex);
		shader->setInt("spotlightsCount", spotlightIndex);

		shader->setFloat4("color", material->color.x, material->color.y, material->color.z, material->color.w);

		vao->bind();

		if (const auto& indexBuffer = vao->getIndexBuffer())
		{
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexBuffer->getStrideCount()), GL_UNSIGNED_INT, nullptr);
		}
		else
		{
			MANI_ASSERT(false, "no index buffer provided with the vertices");
		}

		if (diffuseTexture != nullptr)
		{
			diffuseTexture->unbind();
		}

		if (specularTexture != nullptr)
		{
			specularTexture->unbind();
		}
	}
}

void Mani::OpenGLRenderSystem::getViewport(uint32_t& x, uint32_t& y, uint32_t& width, uint32_t& height)
{
	const OpenGLSystem::WindowContext& context = OpenGLSystem::get().getWindowContext();

	x = 0;
	y = 0;
	width = context.width;
	height = context.height;
}

void Mani::OpenGLRenderSystem::setClearColor(const glm::vec4& color)
{
	m_clearColor = color;
}
