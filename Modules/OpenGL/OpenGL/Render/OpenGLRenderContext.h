#pragma once

#include <Core/Components/Position.h>
#include <Core/Components/Rotation.h>

#include <RenderAPI/Light/DirectionalLight.h>
#include <RenderAPI/Light/PointLight.h>
#include <RenderAPI/Light/Spotlight.h>

#include <OpenGL/Render/IOpenGLRenderer.h>
#include <OpenGL/Render/IOpenGLRenderExtension.h>

#include <ManiMaths/Fwd.h>
#include <vector>

namespace Mani
{
	struct OpenGLWindowContext;

	struct OpenGLRenderContext
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

		std::vector<IOpenGLRenderer*> renderers;
		std::vector<IOpenGLRenderExtension*> extensions;
	};
}
