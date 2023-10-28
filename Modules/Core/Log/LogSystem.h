#pragma once

#include <Core/Core.h>
#include <Core/System/System.h>
#include <string_view>
#include <unordered_map>

namespace ECSEngine
{
	enum class ELogLevel : uint8_t
	{
		Disabled,
		Verbose,
		Log,
		Warning,
		Error,
	};

	class Core_API LogSystem : public SystemBase
	{
	public:
		void log(const std::string_view& channel, ELogLevel level, const std::string_view& log);
		void setChannelLogLevel(const std::string_view& channel, ELogLevel defaultLogLevel);

	private:
		std::unordered_map<std::string_view, ELogLevel> channels;
	};
}
