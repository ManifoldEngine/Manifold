#pragma once

#include <vector>
#include <Inputs/Data/InputControl.h>
#include <string>

namespace ECSEngine
{
	class IInputGenerator
	{
	public:
		virtual std::string getName() const = 0;
		virtual bool consumeInputBuffer(std::vector<ButtonControl>& outBuffer) = 0;
		virtual bool getAxis(std::vector<AxisControl>& outAxis) = 0;
	};
}