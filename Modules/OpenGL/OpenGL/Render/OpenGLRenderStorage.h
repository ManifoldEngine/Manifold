#pragma once

#include <Core/CoreFwd.h>
#include <Core/Async/ThreadPool.h>

#include <OpenGL/Render/IOpenGLRenderer.h>
#include <OpenGL/Render/IOpenGLRenderExtension.h>

namespace Mani
{
    struct OpenGLRenderStorage
    {
        ThreadPool renderThread{ 1 };

        List<IOpenGLRenderer*> renderers;
        List<IOpenGLRenderExtension*> extensions;
    };
}