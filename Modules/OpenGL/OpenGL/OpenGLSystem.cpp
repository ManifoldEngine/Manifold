#include "OpenGLSystem.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <Core/Application.h>
#include <Core/Log.h>
#include <Core/ManiAssert.h>
#include <Core/Debug/Profiling.h>

#include <Resources/ResourceSystem.h>
#include <RenderAPI/ShaderConfig.h>
#include <RenderAPI/Shader.h>

#include <OpenGL/OpenGLDebug.h>
#include <OpenGL/OpenGLConfig.h>
#include <OpenGL/OpenGLWindowContext.h>
#include <OpenGL/OpenGLInputSystem.h>
#include <OpenGL/Data/OpenGLShader.h>
#include <OpenGL/Render/OpenGLResourceSystem.h>
#include <OpenGL/Render/OpenGLCameraUpdateSystem.h>
#include <OpenGL/Render/OpenGLCommandBufferSystem.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <ManiMaths/Fwd.h>

#include <iostream>
#include <vector>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace Mani;

std::string_view OpenGLSystem::getName() const
{
    return "OpenGLSystem";
}

bool OpenGLSystem::shouldTick(const ECS::Registry& registry) const
{
    return false;
}

void OpenGLSystem::onInitialize(ECS::Registry& registry, World& world)
{
#ifndef MANI_COORDINATE_ZMINUSFORWARD_YUP
    // OpenGL uses righ handed z- forward coordinate system.
    static_assert(false);
#endif

    // initialize glfw
    if (!glfwInit())
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to init glfw");
        return;
    }
    world.initializeDependency<ResourceSystem>();
    
    const std::string openglConfigRelPath = std::format("Config/{}", Mani::OPENGLCONFIG_FILENAME);
    const ECS::EntityId openglConfigId = ResourceSystem::loadResourceSync<OpenGLConfig>(registry, openglConfigRelPath);
    const Resource<OpenGLConfig>& configRes = *registry.get<Resource<OpenGLConfig>>(openglConfigId);
    const OpenGLConfig& config = configRes.value;

    // set glfw context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    OpenGLWindowContext& context = *registry.addSingle<OpenGLWindowContext>();
    // create the window
    context.window = glfwCreateWindow(context.width, context.height, context.name.data(), NULL, NULL);

    if (context.window == nullptr)
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to create glfwwindow");
        terminate(registry);
        return;
    }

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    glfwGetWindowSize(context.window, &context.width, &context.height);

    // set this as the window's user pointer. This allows us to retrieve this pointer from the window pointer provided in glfw's callbacks.
    glfwSetWindowUserPointer(context.window, &context);
    glfwMakeContextCurrent(context.window);

    // set glfw callbacks
    glfwSetWindowCloseCallback(context.window, &OpenGLSystem::glfwCallback_onWindowClosed);
    glfwSetFramebufferSizeCallback(context.window, &OpenGLSystem::glfwCallback_onWindowResized);

    // init gl3w to load the correct opengl runtime
    if (gl3wInit() != GL3W_OK)
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to init glew");
        terminate(registry);
        return;
    }
    if (!gl3wIsSupported(3, 2))
    {
        MANI_LOG_ERROR(LogOpenGL, "OpenGL 3.2 is not supported");
        terminate(registry);
        return;
    }

    glfwSwapInterval(config.vsync); // vsync
    loadAndCompileShadersSync(registry, world);

    // set the view port to the window's size.
    glViewport(0, 0, context.width, context.height);
    
#ifndef MANI_WEBGL
    #if MANI_OPENGL_DEBUG   
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    #endif
#endif

    // we leave this context current on the main thread.
    glfwMakeContextCurrent(nullptr);

    world.createSystem<OpenGLResourceSystem>()
        .createSystem<OpenGLInputSystem>()
        .createSystem<OpenGLCameraUpdateSystem>()
        .createSystem<OpenGLCommandBufferSystem>()
        .createSystem<OpenGLRenderSystem>();
}

void OpenGLSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
    terminate(registry);
}

void OpenGLSystem::tick(ECS::Registry& registry)
{
#ifdef __EMSCRIPTEN__
    emscripten_sleep(100);
#endif
}

void OpenGLSystem::terminate(ECS::Registry& registry)
{
    if (OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>())
    {
        glfwDestroyWindow(context->window);
        context->window = nullptr;
    }
    glfwTerminate();
}

// glfw callbacks begin
void OpenGLSystem::glfwCallback_onWindowClosed(GLFWwindow* window)
{
    Application::get().stop();
}

void OpenGLSystem::glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight)
{
    if (auto* context = (OpenGLWindowContext*)glfwGetWindowUserPointer(window))
    {
        context->width = newWidth;
        context->height = newHeight;
    }

    glViewport(0, 0, newWidth, newHeight);
}
// glfw callbacks begin

void Mani::OpenGLSystem::loadAndCompileShadersSync(ECS::Registry& registry, World& world)
{
    namespace fs = std::filesystem;
    world.initializeDependency<ResourceSystem>();
    world.initializeDependency<OpenGLResourceSystem>();

    // load shader config file
    const fs::path configPath = FileSystem::getConfigPath().append(Mani::SHADERCONFIG_FILENAME);
    const ECS::EntityId configId = ResourceSystem::loadResourceSync<ShaderConfig>(registry, configPath);
    const ShaderConfig& config = registry.get<Resource<ShaderConfig>>(configId)->value;

    // load the shader collection json generated by the asset importer
    const fs::path collectionPath = FileSystem::getAbsolutePath(config.shaderCollectionRelativePath).append(Mani::SHADERCOLLECTION_FILENAME);
    const ECS::EntityId collectionId = ResourceSystem::loadResourceSync<ShaderCollection>(registry, collectionPath);
    const ShaderCollection& collection = registry.get<Resource<ShaderCollection>>(collectionId)->value;

    for (const auto& shaderPath : collection.paths)
    {
        // load shader
        ResourceSystem::loadResourceSync<Shader>(registry, shaderPath);
    }
}