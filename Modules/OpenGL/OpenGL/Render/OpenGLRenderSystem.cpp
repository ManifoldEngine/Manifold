#include "OpenGLRenderSystem.h"

#include <Core/Debug/Profiling.h>
#include <Core/Async/ThreadPool.h>

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

	unsigned int readBuffer3D = 0;
	unsigned int readBuffer2D = 0;
};

RenderContext createContext(const ECS::Registry& registry);

void render3d(const OpenGLCommand3D& command, RenderContext& context);
void render2d(const OpenGLCommand2D& command, RenderContext& context);
void loadQuad(uint32_t repeatAmount, Resource<OpenGLVertexArray>& res);

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

	OpenGLCommandBuffer3D* cbs3d = registry.getSingle<OpenGLCommandBuffer3D>();
	if (cbs3d == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Trying to push opengl commands without a command buffer");
		return;
	}

	/*OpenGLCommandBuffer2D* cbs2d = registry.getSingle<OpenGLCommandBuffer2D>();
	if (cbs2d == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Trying to push opengl commands without a command buffer");
		return;
	}*/

	if (!cbs3d->isReadBufferValid() /* || !cbs2d->isReadBufferValid()*/)
	{
		return;
	}

	OpenGLRenderSystem::Storage& storage = *registry.getSingle<OpenGLRenderSystem::Storage>();
	RenderContext context = createContext(registry);
	context.readBuffer3D = cbs3d->readBuffer;

#if MANI_DEBUG
	const int fps = Math::isEqual(deltaTime, 0.f) ? 0 : static_cast<int>(1 / deltaTime);
	glfwSetWindowTitle(context.openglContext->window, std::format("{} ({}fps)", context.openglContext->name, fps).c_str());
#endif

	storage.renderThread.enqueue([&registry, cbs3d, context = std::move(context)]() mutable
	{
		MANI_TIME_SCOPE(OpenGLRenderSystemtickrenderthread);
		glfwMakeContextCurrent(context.openglContext->window);

		std::vector<OpenGLCommand3D>& commands3d = cbs3d->buffers[context.readBuffer3D];
		glEnable(GL_DEPTH_TEST);

		// setting color state.
		const Vec4f& clearColor = context.clearColor;
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

		// consuming color state.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, context.width, context.height);

		for (const auto& command3d : commands3d)
		{
			render3d(command3d, context);
		}

		//for (const auto& command2d : commands2d)
		//{
		//	render2d(command2d, context);
		//}

		// render extension if any
		for (const auto entityId : ECS::View<OpenGLRenderExtension>(registry))
		{
			OpenGLRenderExtension& ext = *registry.get<OpenGLRenderExtension>(entityId);
			ext.obj->onPostRender(registry);
		}

		cbs3d->renderFrame++;
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

void render3d(const OpenGLCommand3D& command, RenderContext& context)
{
	MANI_TIME_SCOPE(OpenGLRenderSystemtickrenderthreadrender);

	MANI_ASSERT(command.vao != nullptr, "Resource was unloaded unexpectedly.");
	MANI_ASSERT(command.shader != nullptr, "Resource was unloaded unexpectedly.");

	const Mat4f& model = command.model;
	const OpenGLVertexArray& vao = *command.vao;
	const OpenGLShader& shader = *command.shader;
	
	const Mat3f normalMatrix = static_cast<Mat3f>(model).inverse().transpose();

	shader.use();
	// set vertex uniforms
	shader.setFloatMatrix4(MODEL, &(model._00));
	shader.setFloatMatrix3(NORMALMATRIX, &(normalMatrix._00));
	shader.setFloatMatrix4(VIEW, &(context.view._00));
	shader.setFloatMatrix4(PROJECTION, &(context.projection._00));

	// set fragment uniforms
	shader.setFloat3(VIEWPOSITION, context.cameraPosition.x, context.cameraPosition.y, context.cameraPosition.z);

	shader.setFloat(MATERIAL_SHININESS, command.shininess);
	
	const Vec4f& color = command.color;
	shader.setFloat4(COLOR, color.x, color.y, color.z, color.w);

	int textureIndex = 0;
	if (command.diffuse != nullptr)
	{
		command.diffuse->bind(textureIndex++);
		shader.setTextureSlot(MATERIAL_DIFFUSE, textureIndex);
	}

	if (command.specular != nullptr)
	{
		command.specular->bind(textureIndex++);
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

	vao.bind();

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vao.getIndexBuffer().getStrideCount()), GL_UNSIGNED_INT, nullptr);

	if (command.diffuse != nullptr)
	{
		command.diffuse->unbind();
	}

	if (command.specular != nullptr)
	{
		command.specular->unbind();
	}
}

void render2d(const OpenGLCommand2D& command, RenderContext& context)
{

}

void loadQuad(uint32_t repeatAmount, Resource<OpenGLVertexArray>& res)
{
	const float repeatAmountF = static_cast<float>(repeatAmount);

	// hardcoded 2d quad. We flip the X axis because OpenGL is right handed.
	const std::vector<float> vertices =
	{
		//    vertex		//        texture
		0.0f, 0.0f, 1.0f,	0.0f,			repeatAmountF,
		-1.0f, 0.0f, 0.0f,	repeatAmountF,	0.0f,
		0.0f, 0.0f, 0.0f,	0.0f,			0.0f,
		0.0f, 0.0f, 1.0f,	0.0f,			repeatAmountF,
		-1.0f, 0.0f, 1.0f,	repeatAmountF,	repeatAmountF,
		-1.0f, 0.0f, 0.0f,	repeatAmountF,	0.0f,
	};

	OpenGLVertexBuffer vertexBuffer = OpenGLVertexBuffer(&vertices[0], (int)(sizeof(float) * vertices.size()));
	vertexBuffer.layout =
	{
		{ EShaderDataType::Float3, false },
		{ EShaderDataType::Float2, false }
	};

	std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5 };
	OpenGLIndexBuffer indexBuffer = OpenGLIndexBuffer(&indices[0], (int)sizeof(uint32_t) * indices.size());

	res.value.addVertexBuffer(std::move(vertexBuffer));
	res.value.setIndexBuffer(std::move(indexBuffer));
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
