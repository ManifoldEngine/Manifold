#pragma once

#include <string_view>

namespace Mani
{
	class STBITexture
	{
	public:
		bool load(const std::string_view& path);
		bool isLoaded() const { return data != nullptr; }
		void freeTexture();
		
		int width = 0;
		int height = 0;
		int channels = 0;
		unsigned char* data = nullptr;
	};
}