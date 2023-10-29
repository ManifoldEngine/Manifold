#pragma once

#include <Core/Core.h>
#include <Core/System/System.h>
#include <string_view>

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
		LogSystem();
		~LogSystem();

		void log(const std::string_view& channel, ELogLevel level, const std::string_view& log);
		void setChannelLogLevel(const std::string_view& channel, ELogLevel logLevel);

	private:
		struct Impl;
		Impl* m_pImpl = nullptr;
	};
}