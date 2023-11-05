#pragma once

#include <OpenGL/OpenGL.h>
#include <GL/glew.h>

namespace ECSEngine
{
	void OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
}