#pragma once

#include <Core/ManiAssert.h>
#include <Inputs/Data/InputControl.h>

namespace Mani
{
	struct UniqueControlIdGenerator
	{
		ControlId generateNextId() 
		{ 
			MANI_ASSERT(m_nextControlId != INVALID_CONTROL_ID, "There no more control ids available");
			return m_nextControlId++; 
		}
	private:
		ControlId m_nextControlId = 0;
	};
}