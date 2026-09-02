#pragma once

#include <ManiTypes.h>
#include <RenderAPI/Font.h>

namespace Mani
{
	namespace STB
	{
		bool loadFont(const Path& absolutePath, Font& font);
		bool unloadFont(Font& font);
	}
}