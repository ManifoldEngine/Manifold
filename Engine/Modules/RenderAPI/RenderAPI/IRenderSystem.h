#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class IRenderSystem
	{
	public:
		virtual void getViewport(uint32_t& x, uint32_t& y, uint32_t& width, uint32_t& height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
	};
}