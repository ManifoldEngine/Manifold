#pragma once
#include <string_view>

#ifdef OSUtils_EXPORTS
#define OSUtils_API __declspec(dllexport)
#else
#define OSUtils_API
#endif

#if defined _WIN32 || defined _WIN64
#define ECSEngine_FULL_FUNCTION_SIG __FUNCSIG__
#else
#define ECSEngine_FULL_FUNCTION_SIG "Platform not supported."
#endif

namespace ECSEngine {
	template<typename T>
	OSUtils_API constexpr auto GetTypeName() {
		// get full signature (this is os/compiler specific)
		const std::string_view functionName{ ECSEngine_FULL_FUNCTION_SIG };
		
		// get first "<"
		auto first = functionName.find_first_not_of(' ', functionName.find_first_of("<") + 1);

		// find the substring in between the firs "<" and last ">"
		return functionName.substr(first, functionName.find_last_of(">") - first);
	}
	
	template<typename T>
	OSUtils_API constexpr auto GetTypeHash() {
		return std::hash<std::string>(GetTypeName<T>());
	}
}