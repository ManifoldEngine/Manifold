#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class IOpenGLRenderExtension
	{
	public:
		virtual void onPostRender(ECS::Registry& registry) const = 0;
	};

	struct OpenGLRenderExtension
	{
		std::shared_ptr<IOpenGLRenderExtension> obj;
	};
}