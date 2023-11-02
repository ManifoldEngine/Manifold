#pragma once

#include <string>
#include <algorithm>

namespace ECSEngine
{
	char asciiToLower(char c)
	{
		if (c <= 'Z' && c >= 'A')
		{
			return c - ('Z' - 'z');
		}
		return c;
	};

	std::string toLower(const std::string& s)
	{
		std::string result = s;
		std::transform(result.begin(), result.end(), result.begin(), asciiToLower);
		return result;
	}

	char asciiToUpper(char c)
	{
		if (c >= 'a' && c <= 'z')
		{
			return c + ('A' - 'a');
		}
		return c;
	}

	std::string toUpper(const std::string& s)
	{
		std::string result = s;
		std::transform(result.begin(), result.end(), result.begin(), asciiToUpper);
		return result;
	}

	std::string removeWhiteSpace(std::string s)
	{
		std::string result;
		for (const char c : s)
		{
			if (!isspace(c))
			{
				result += c;
			}
		}
		return result;
	}
}