#pragma once

#include <fmod.hpp>
#include <string>

namespace Mani
{
	constexpr std::string_view LogFMod = "FMod";

	struct FMod
	{
		FMOD::System* system = nullptr;
	};
}