#pragma once

#include <Core/ManiTypes.h>
#include <Core/Containers/List.h>
#include <RenderAPI/Font.h>

#include <stb_truetype.h>

namespace Mani
{
	namespace STB
	{
		struct STBFontData
		{
			Mani::List<unsigned char> data;
			::stbtt_fontinfo info;
		};

		bool loadFont(const Path& absolutePath, Font& font);
		bool unloadFont(Font& font);
	}
}