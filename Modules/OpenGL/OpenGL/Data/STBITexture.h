#pragma once

#include <string_view>

namespace Mani
{
	class STBITexture
	{
	public:
		STBITexture(const std::string_view& path);
		~STBITexture();

		int width;
		int height;
		int channels;
		unsigned char* data;
	};
}