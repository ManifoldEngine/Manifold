#pragma once

#include "ManiTests.h"

#include <Core/Logger.h>
#include <ManiTests/ManiTests.h>

namespace Mani
{
	void runTests()
	{
		Mani::Logger::get().suppress();
		ManiTests::run();
		Mani::Logger::get().unsuppress();
	}
}
