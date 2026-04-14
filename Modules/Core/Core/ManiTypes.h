#pragma once

#include <ManiMaths/Traits.h>
#include <Core/ManiTraits.h>
#include <limits>
#include <thread>
#include <filesystem>

namespace Mani
{
	using SizeT = std::size_t;
	inline constexpr SizeT INDEX_NONE = (std::numeric_limits<SizeT>::max)();

	using Path = std::filesystem::path;

	using ThreadId = std::thread::id;
	[[nodiscard]] inline ThreadId thisThreadId() noexcept { return std::this_thread::get_id(); }
}