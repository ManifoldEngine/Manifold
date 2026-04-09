#pragma once

#include <Core/CoreFwd.h>

#include <OpenGL/Render/IOpenGLRenderer.h>
#include <OpenGL/Render/IOpenGLRenderExtension.h>

namespace Mani
{
    struct OpenGLRenderStorage
    {
        class ThreadPool* renderThread = nullptr;

        List<IOpenGLRenderer*> renderers;
        List<IOpenGLRenderExtension*> extensions;
    };
}