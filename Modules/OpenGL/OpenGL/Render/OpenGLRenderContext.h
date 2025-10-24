#pragma once

#include <Core/Containers/List.h>
#include <Core/Components/Position.h>
#include <Core/Components/Rotation.h>

#include <RenderAPI/Light/DirectionalLight.h>
#include <RenderAPI/Light/PointLight.h>
#include <RenderAPI/Light/Spotlight.h>
#include <RenderAPI/RenderContext.h>

#include <OpenGL/Render/IOpenGLRenderer.h>
#include <OpenGL/Render/IOpenGLRenderExtension.h>

#include <ManiMaths/Fwd.h>

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
		List<DirectionalLight> directionalLights;
		List<std::tuple<PointLight, Position>> pointLights;
		List<std::tuple<Spotlight, Position, Rotation>> spotlights;

		List<IOpenGLRenderer*> renderers;
		List<IOpenGLRenderExtension*> extensions;

		RenderContext renderContext;
	};
}
