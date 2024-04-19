#pragma once

#include <vector>
#include <Inputs/Data/InputControl.h>
#include <string>

namespace Mani
{
	class EntityRegistry;

	class IInputGenerator
	{
	public:
		virtual ~IInputGenerator() {}
		virtual std::string getName() const = 0;
		virtual bool consumeInputBuffer(std::vector<ButtonControl>& outBuffer) = 0;
		virtual bool getAxis(std::vector<AxisControl>& outAxis) = 0;
		virtual void onInputSystemTick(float deltaTime, EntityRegistry& registry) = 0;
	};
}