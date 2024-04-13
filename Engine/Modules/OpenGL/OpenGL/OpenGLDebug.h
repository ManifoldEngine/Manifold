#pragma once

#ifndef MANI_WEBGL
#include <OpenGL/OpenGL.h>
#include <GL/glew.h>

namespace Mani
{
	void OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
}
#endif