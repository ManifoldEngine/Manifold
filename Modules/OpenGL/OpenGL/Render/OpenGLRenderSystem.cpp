#include "OpenGLRenderSystem.h"

#include <Core/Debug/Profiling.h>
#include <Core/Thread/ThreadPool.h>

#include <Camera/CameraSystem.h>

#include <Resources/Resource.h>
#include <Resources/ResourceSystem.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/OpenGLWindowContext.h>

#include <OpenGL/Data/OpenGLVertexArray.h>
#include <OpenGL/Data/OpenGLBuffer.h>
#include <OpenGL/Data/OpenGLClearColor.h>
#include <OpenGL/Data/OpenGLMaterial.h>
#include <OpenGL/Data/OpenGLShader.h>
#include <OpenGL/Data/OpenGLTexture.h>
#include <OpenGL/Data/STBITexture.h>

#include <OpenGL/Render/OpenGLResourceSystem.h>
#include <OpenGL/Render/OpenGLCameraUpdateSystem.h>
#include <OpenGL/Render/OpenGLCommand.h>
#include <OpenGL/Render/IOpenGLRenderExtension.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/Shader.h>
#include <RenderAPI/Texture.h>
#include <RenderAPI/Light/DirectionalLight.h>
#include <RenderAPI/Light/PointLight.h>
#include <RenderAPI/Light/Spotlight.h>

#include <GLFW/glfw3.h>

using namespace Mani;

// shader parameters
constexpr std::string_view MODEL = "model";
constexpr std::string_view NORMALMATRIX = "normalMatrix";
constexpr std::string_view VIEW = "view";
constexpr std::string_view PROJECTION = "projection";
constexpr std::string_view VIEWPOSITION = "viewPosition";
constexpr std::string_view COLOR = "color";
constexpr std::string_view MATERIAL_SHININESS = "material.shininess";
constexpr std::string_view MATERIAL_DIFFUSE = "material.diffuseMap";
constexpr std::string_view MATERIAL_SPECULAR = "material.specularMap";

constexpr std::string_view DIRECTIONALLIGHTS = "directionalLights";
constexpr std::string_view _DIRECTION = ".direction";
constexpr std::string_view _AMBIENT = ".ambient";
constexpr std::string_view _DIFFUSE = ".diffuse";
constexpr std::string_view _SPECULAR = ".specular";

constexpr std::string_view POINTLIGHTS = "pointLights";
constexpr std::string_view _POSITION = ".position";
constexpr std::string_view _CONSTANT = ".constant";
constexpr std::string_view _LINEAR = ".linear";
constexpr std::string_view _QUADRATIC = ".quadratic";

constexpr std::string_view SPOTLIGHTS = "spotlights";
constexpr std::string_view _CUTOFF = ".cutOff";
constexpr std::string_view _OUTTERCUTOFF = ".outterCutOff";

constexpr std::string_view DIRECTIONALLIGHTSCOUNT = "directionalLightsCount";
constexpr std::string_view POINTLIGHTSCOUNT = "pointLightsCount";
constexpr std::string_view SPOTLIGHTSCOUNT = "spotlightsCount";

struct RenderContext
{
	const OpenGLWindowContext* openglContext = nullptr;
	
	// camera
	Mat4f view = MAT4F::IDENTITY;
	Mat4f projection = MAT4F::IDENTITY;
	Vec3f cameraPosition = VEC3F::ZERO;
	int width = 0;
	int height = 0;

	Vec4f clearColor = { 0.f, 0.f, 0.f, 1.f };

	// light
	std::vector<DirectionalLight> directionalLights;
	std::vector<std::tuple<PointLight, Position>> pointLights;
	std::vector<std::tuple<Spotlight, Position, Rotation>> spotlights;

	OpenGLRenderSystem::Storage* storage;
};

RenderContext createContext(const ECS::Registry& registry);

void render(const OpenGLCommand3D& command, RenderContext& context);
void loadOpenGLVertexArray(const Mesh& meshRes, Resource<OpenGLVertexArray>& res);
void loadOpenGLTexture(const STBITexture& stbiTexture, Resource<OpenGLTexture2D>& res);

struct OpenGLRenderSystem::Storage
{
	ThreadPool renderThread{ 1 };

	std::unordered_map<ECS::EntityId, Resource<OpenGLVertexArray>> vaos;
	std::unordered_map<ECS::EntityId, Resource<OpenGLTexture2D>> textures;

	std::mutex resourceMutex;
};

void OpenGLRenderSystem::onInitialize(ECS::Registry& registry, World& world)
{
	world.initializeDependency<CameraSystem>();
	world.initializeDependency<OpenGLCameraUpdateSystem>();
	
	registry.addSingle<OpenGLRenderSystem::Storage>();
	registry.addSingle<OpenGLClearColor>();
}

void OpenGLRenderSystem::onDeinitialize(ECS::Registry& registry)
{
	OpenGLRenderSystem::Storage& storage = *registry.getSingle<OpenGLRenderSystem::Storage>();
	storage.renderThread.stop();

	registry.removeSingle<OpenGLClearColor>();
	registry.removeSingle<OpenGLRenderSystem::Storage>();
}

void OpenGLRenderSystem::tick(float deltaTime, ECS::Registry& registry)
{
	MANI_TIME_SCOPE(OpenGLRenderSystemtick);

	OpenGLCommandBuffer3D* cbs = registry.getSingle<OpenGLCommandBuffer3D>();
	if (cbs == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Trying to push opengl commands without a command buffer");
		return;
	}

	if (!cbs->isReadBufferValid())
	{
		return;
	}

	OpenGLRenderSystem::Storage& storage = *registry.getSingle<OpenGLRenderSystem::Storage>();
	RenderContext context = createContext(registry);
	context.storage = &storage;
	
#if MANI_DEBUG
	const float fps = Math::isEqual(deltaTime, 0.f) ? 0.f : 1 / deltaTime;
	glfwSetWindowTitle(context.openglContext->window, std::format("{} ({}fps)", context.openglContext->name, fps).c_str());
#endif

	const unsigned int readBuffer = cbs->readBuffer;
	storage.renderThread.enqueue([&registry, cbs, readBuffer, context = std::move(context)]() mutable
	{
		MANI_TIME_SCOPE(OpenGLRenderSystemtickrenderthread);
		glfwMakeContextCurrent(context.openglContext->window);

		std::vector<OpenGLCommand3D>& commands = cbs->buffers[readBuffer];

		glEnable(GL_DEPTH_TEST);

		// setting color state.
		const Vec4f& clearColor = context.clearColor;
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

		// consuming color state.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, context.width, context.height);

		for (const auto& command : commands)
		{
			render(command, context);
		}

		// render extension if any
		for (const auto entityId : ECS::View<OpenGLRenderExtension>(registry))
		{
			OpenGLRenderExtension& ext = *registry.get<OpenGLRenderExtension>(entityId);
			ext.obj->onPostRender(registry);
		}

		cbs->renderFrame++;
		glfwMakeContextCurrent(nullptr);
		glfwSwapBuffers(context.openglContext->window);
	});
}

RenderContext createContext(const ECS::Registry& registry)
{
	RenderContext context;
	{
		context.openglContext = registry.getSingle<OpenGLWindowContext>();
	}

	{
		// camera
		ECS::View<Position, Camera> cameraView(registry);
		auto it = cameraView.begin();
		MANI_ASSERT(it != cameraView.end(), "Trying to render without a camera");

		const Position& position = *registry.get<Position>(*it);
		context.cameraPosition = position.value;

		const Camera& camera = *registry.get<Camera>(*it);
		context.view = camera.view;
		context.projection = camera.projection;
		context.width = static_cast<int>(camera.width);
		context.height = static_cast<int>(camera.height);
	}

	{
		// light
		for (const auto entityId : ECS::View<DirectionalLight> (registry))
		{
			const DirectionalLight& light = *registry.get<DirectionalLight>(entityId);
			context.directionalLights.emplace_back(light);
		}
		for (const auto entityId : ECS::View<PointLight, Position> (registry))
		{
			const PointLight& light = *registry.get<PointLight>(entityId);
			const Position& position = *registry.get<Position>(entityId);
			context.pointLights.emplace_back(std::tuple<PointLight, Position>{ light, position });
		}
		for (const auto entityId : ECS::View<Spotlight, Position, Rotation> (registry))
		{
			const Spotlight& light = *registry.get<Spotlight>(entityId);
			const Position& position = *registry.get<Position>(entityId);
			const Rotation& rotation = *registry.get<Rotation>(entityId);
			context.spotlights.emplace_back(std::tuple<Spotlight, Position, Rotation>{ light, position, rotation });
		}
	}

	{
		// clear color
		const OpenGLClearColor& clearColor = *registry.getSingle<OpenGLClearColor>();
		context.clearColor = clearColor.color;
	}

	return context;
}

void render(const OpenGLCommand3D& command, RenderContext& context)
{
	MANI_TIME_SCOPE(OpenGLRenderSystemtickrenderthreadrender);

	Resource<OpenGLVertexArray>* vaoRes = nullptr;
	{
		std::lock_guard<std::mutex> lock(context.storage->resourceMutex);
		vaoRes = &(context.storage->vaos[command.meshId]);
	}

	if (!vaoRes->isReady)
	{
		loadOpenGLVertexArray(*command.mesh, *vaoRes);
	}

	const Material& material = *command.material;
	const OpenGLShader& shader = *command.shader;

	const Mat3f normalMatrix = static_cast<Mat3f>(command.model).inverse().transpose();

	shader.use();
	// set vertex uniforms
	shader.setFloatMatrix4(MODEL, &(command.model._00));
	shader.setFloatMatrix3(NORMALMATRIX, &(normalMatrix._00));
	shader.setFloatMatrix4(VIEW, &(context.view._00));
	shader.setFloatMatrix4(PROJECTION, &(context.projection._00));

	// set fragment uniforms
	shader.setFloat3(VIEWPOSITION, context.cameraPosition.x, context.cameraPosition.y, context.cameraPosition.z);

	shader.setFloat(MATERIAL_SHININESS, material.shininess);
	shader.setFloat4(COLOR, material.color.x, material.color.y, material.color.z, material.color.w);

	int textureIndex = 0;
	Resource<OpenGLTexture2D>* diffuseRes = nullptr;
	if (command.diffuse != nullptr)
	{
		{
			std::lock_guard<std::mutex> lock(context.storage->resourceMutex);
			diffuseRes = &(context.storage->textures[command.diffuseId]);
		}

		loadOpenGLTexture(*command.diffuse, *diffuseRes);
		
		OpenGLTexture2D& texture = diffuseRes->getMutable();
		texture.bind(textureIndex);
		shader.setTextureSlot(MATERIAL_DIFFUSE, textureIndex);
	}

	Resource<OpenGLTexture2D>* specularRes = nullptr;
	if (command.specular != nullptr)
	{
		{
			std::lock_guard<std::mutex> lock(context.storage->resourceMutex);
			specularRes = &(context.storage->textures[command.specularId]);
		}

		loadOpenGLTexture(*command.specular, *specularRes);

		OpenGLTexture2D& texture = specularRes->getMutable();
		texture.bind(textureIndex);
		shader.setTextureSlot(MATERIAL_SPECULAR, textureIndex);
	}

	// set lights
	int directionalLightIndex = 0;
	int pointLightIndex = 0;
	int spotlightIndex = 0;
	for(const DirectionalLight& light : context.directionalLights)
	{
		const std::string accessor = std::format("{}[{}]", DIRECTIONALLIGHTS, directionalLightIndex);
		shader.setFloat3(std::format("{}{}", accessor, _DIRECTION).c_str(), light.direction.x, light.direction.y, light.direction.z);
		shader.setFloat3(std::format("{}{}", accessor, _AMBIENT).c_str(), light.ambient.x, light.ambient.y, light.ambient.z);
		shader.setFloat3(std::format("{}{}", accessor, _DIFFUSE).c_str(), light.diffuse.x, light.diffuse.y, light.diffuse.z);
		shader.setFloat3(std::format("{}{}", accessor, _SPECULAR).c_str(), light.specular.x, light.specular.x, light.specular.x);
		directionalLightIndex++;
	}

	for (const auto& [light, position] : context.pointLights)
	{
		const std::string accessor = std::format("{}[{}]", POINTLIGHTS, pointLightIndex);
		shader.setFloat3(std::format("{}{}", accessor, _POSITION).c_str(), position.value.x, position.value.y, position.value.z);

		shader.setFloat3(std::format("{}{}", accessor, _AMBIENT).c_str(), light.ambient.x, light.ambient.y, light.ambient.z);
		shader.setFloat3(std::format("{}{}", accessor, _DIFFUSE).c_str(), light.diffuse.x, light.diffuse.y, light.diffuse.z);
		shader.setFloat3(std::format("{}{}", accessor, _SPECULAR).c_str(), light.specular.x, light.specular.x, light.specular.x);

		shader.setFloat(std::format("{}{}", accessor, _CONSTANT).c_str(), light.constant);
		shader.setFloat(std::format("{}{}", accessor, _LINEAR).c_str(), light.linear);
		shader.setFloat(std::format("{}{}", accessor, _QUADRATIC).c_str(), light.quadratic);

		pointLightIndex++;
	}

	for (const auto& [light, position, rotation] : context.spotlights)
	{
		const Vec3f forward = Transform::forward(rotation);

		const std::string accessor = std::format("{}[{}]", SPOTLIGHTS, spotlightIndex);
		shader.setFloat3(std::format("{}{}", accessor, _POSITION).c_str(), position.value.x, position.value.y, position.value.z);
		shader.setFloat3(std::format("{}{}", accessor, _DIRECTION).c_str(), forward.x, forward.y, forward.z);
		shader.setFloat(std::format("{}{}", accessor, _CUTOFF).c_str(), light.cutOff);
		shader.setFloat(std::format("{}{}", accessor, _OUTTERCUTOFF).c_str(), light.outterCutOff);

		shader.setFloat3(std::format("{}{}", accessor, _AMBIENT).c_str(), light.ambient.x, light.ambient.y, light.ambient.z);
		shader.setFloat3(std::format("{}{}", accessor, _DIFFUSE).c_str(), light.diffuse.x, light.diffuse.y, light.diffuse.z);
		shader.setFloat3(std::format("{}{}", accessor, _SPECULAR).c_str(), light.specular.x, light.specular.x, light.specular.x);

		spotlightIndex++;
	}

	shader.setInt(DIRECTIONALLIGHTSCOUNT, directionalLightIndex);
	shader.setInt(POINTLIGHTSCOUNT, pointLightIndex);
	shader.setInt(SPOTLIGHTSCOUNT, spotlightIndex);

	const OpenGLVertexArray& vao = vaoRes->get();
	vao.bind();

	if (const auto& indexBuffer = vao.getIndexBuffer())
	{
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexBuffer->getStrideCount()), GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		MANI_ASSERT(false, "no index buffer provided with the vertices");
	}

	if (diffuseRes != nullptr)
	{
		diffuseRes->getMutable().unbind();
	}

	if (specularRes != nullptr)
	{
		specularRes->getMutable().unbind();
	}
}

void loadOpenGLVertexArray(const Mesh& mesh, Resource<OpenGLVertexArray>& res)
{
	constexpr size_t vertexSize = 3 + 3 + 2;
	std::shared_ptr<OpenGLVertexBuffer> vertexBuffer = std::make_shared<OpenGLVertexBuffer>(&mesh.vertices[0].position.x, (int)(sizeof(float) * (mesh.vertices.size() * vertexSize)));;
	vertexBuffer->layout =
	{
		{ EShaderDataType::Float3, false },
		{ EShaderDataType::Float3, true  },
		{ EShaderDataType::Float2, false }
	};

	std::shared_ptr<OpenGLIndexBuffer> indexBuffer = std::make_shared<OpenGLIndexBuffer>(&mesh.indices[0], (int)sizeof(uint32_t) * mesh.indices.size());

	res.value = std::make_unique<OpenGLVertexArray>();
	res.value->addVertexBuffer(vertexBuffer);
	res.value->setIndexBuffer(indexBuffer);

	res.isReady = true;
}

void loadOpenGLTexture(const STBITexture& stbiTexture, Resource<OpenGLTexture2D>& res)
{
	res.value = std::make_shared<OpenGLTexture2D>(stbiTexture);
	res.isReady = true;
}


ECS::EntityId OpenGLRenderSystem::addExtension(ECS::Registry& registry, std::shared_ptr<IOpenGLRenderExtension> extension)
{
	ECS::EntityId entityId = registry.create();
	OpenGLRenderExtension& ext = *registry.add<OpenGLRenderExtension>(entityId);
	ext.obj = extension;
	return entityId;
}

void OpenGLRenderSystem::removeExtension(ECS::Registry& registry, ECS::EntityId entityId)
{
	registry.destroy(entityId);
}
