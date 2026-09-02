#include "STBTruetype.h"

#include <Core/ManiAssert.h>
#include <Core/FileSystem.h>

#include <STB/STB.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

using namespace Mani;

constexpr std::string_view fileType = ".ttf";

struct STBFontData
{
	Mani::List<unsigned char> data;
	stbtt_fontinfo info;
};

bool STB::loadFont(const Path& absolutePath, Font& font)
{
	Mani::List<unsigned char> bytes;
	MANI_ASSERT(absolutePath.extension().string() == fileType, "Unsupported file type {}", absolutePath.extension().string());
	if (!Mani::FileSystem::readBytes(absolutePath, bytes))
	{
		MANI_LOG_ERROR(STB::Log, "Could not open font file at {}", absolutePath.string());
		return false;
	}

	STBFontData* fontData = new STBFontData();
	fontData->data = bytes;
	const unsigned char* dataPtr = &fontData->data[0];

	if (!stbtt_InitFont(&fontData->info, dataPtr, stbtt_GetFontOffsetForIndex(dataPtr, 0)))
	{
		MANI_LOG_ERROR(STB::Log, "Could not load font file {}", absolutePath.string());
		delete fontData;
		return false;
	}

	font.buffer = (unsigned char*)fontData;
	return true;
}

bool STB::unloadFont(Font& font)
{
	if (font.buffer == nullptr)
	{
		return false;
	}

	STBFontData* fontData = (STBFontData*)font.buffer;
	delete fontData;
	font.buffer = nullptr;
	return true;
}
