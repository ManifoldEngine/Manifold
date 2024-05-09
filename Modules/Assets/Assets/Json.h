#pragma once

#include <string>
#include <rfl/json.hpp>

template<typename T>
struct Json
{
	static T fromJson(const std::string& content);
	static std::string toJson(const T& data);
};

template<typename T>
inline T Json<T>::fromJson(const std::string& content)
{
	return rfl::json::read<T>(content).value();
}

template<typename T>
std::string Json<T>::toJson(const T& data)
{
	return rfl::json::write(data);
}
