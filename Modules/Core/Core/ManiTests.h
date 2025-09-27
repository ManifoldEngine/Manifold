#pragma once

#include "ManiTests.h"

#include <Core/Log/LogSystem.h>
#include <ManiTests/ManiTests.h>

namespace Mani
{
	void runTests()
	{
		LogSystem::s_suppress();
		ManiTests::run();
		LogSystem::s_unsuppress();
	}
}
